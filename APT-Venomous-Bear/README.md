# Simulasi Adversary APT Venomous Bear

Ini adalah simulasi serangan oleh grup APT (Venomous Bear) yang menargetkan Amerika Serikat, Jerman, dan Afghanistan. Kampanye serangan ini aktif setidaknya sejak tahun 2020. Rantai serangan dimulai dengan memasang backdoor sebagai layanan di mesin yang terinfeksi. Para penyerang mencoba beroperasi secara diam-diam dengan menamai layanan sebagai "Windows Time Service", menyerupai layanan Windows yang sudah ada. Backdoor ini dapat mengunggah dan mengeksekusi file atau mengekstrak file dari sistem yang terinfeksi. Backdoor ini berkomunikasi dengan server command and control (C2) melalui saluran terenkripsi HTTPS setiap lima detik untuk memeriksa apakah ada perintah baru dari operator. Saya mengandalkan informasi dari Cisco Talos Intelligence Group untuk menyusun simulasi ini: https://blog.talosintelligence.com/tinyturla/

![imageedit_3_4790485345](https://github.com/S3N4T0R-0X0/Venomous-Bear-APT/assets/121706460/1a56bebb-927d-4286-8257-aa907f240017)

Para penyerang menggunakan file .BAT yang menyerupai Layanan Waktu Windows Microsoft untuk memasang backdoor. Backdoor ini berbentuk library link dinamis layanan (DLL) yang disebut w64time.dll. Deskripsi dan nama file membuatnya terlihat seperti DLL Microsoft yang valid. Setelah aktif dan berjalan, backdoor ini memungkinkan penyerang mengekstrak file atau mengunggah dan mengeksekusi file, sehingga berfungsi sebagai jalur masuk tahap kedua sesuai kebutuhan.

1. **File BAT**: Para penyerang menggunakan file .bat seperti yang ditunjukkan di bawah ini untuk memasang backdoor sebagai layanan Microsoft Windows Time palsu.

2. **Backdoor DLL**: Saya telah mengembangkan simulasi backdoor yang digunakan para penyerang dalam serangan asli.

3. **Pendengar Backdoor**: Saya juga mengembangkan skrip pendengar sederhana yang menunggu koneksi masuk dari backdoor saat dijalankan di mesin target.

Menurut tim Cisco, mereka tidak dapat mengidentifikasi metode yang digunakan untuk memasang backdoor ini di sistem korban.

![Screenshot from 2024-06-09 16-11-23](https://github.com/S3N4T0R-0X0/Venomous-Bear-APT/assets/121706460/3116c5e9-0476-4b93-a672-bc7436abfce0)

## Tahap Pertama (File .BAT)

Para penyerang menggunakan file .bat seperti yang ditunjukkan di bawah ini untuk memasang backdoor sebagai layanan Microsoft Windows Time palsu. File .bat ini juga mengatur parameter konfigurasi di registry yang digunakan backdoor.

![Screenshot from 2024-06-07 19-39-16](https://github.com/S3N4T0R-0X0/Venomous-Bear-APT/assets/121706460/381d1833-3f71-4278-aa56-60952e8d3f55)

Saya menulis file .bat yang identik dengan yang digunakan para penyerang untuk memasang backdoor sebagai layanan Microsoft Windows Time palsu.

Perintah-perintah ini menambahkan berbagai parameter konfigurasi untuk layanan W64Time ke registry.

    reg add "HKLM\SYSTEM\CurrentControlSet\services\W64Time\Parameters" /v ServiceDll /t REG_EXPAND_SZ /d "%SystemRoot%\system32\w64time.dll" /f
    reg add "HKLM\SYSTEM\CurrentControlSet\services\W64Time\Parameters" /v Hosts /t REG_SZ /d "REMOVED 5050" /f
    reg add "HKLM\SYSTEM\CurrentControlSet\services\W64Time\Parameters" /v Security /t REG_SZ /d "<REMOVED>" /f
    reg add "HKLM\SYSTEM\CurrentControlSet\services\W64Time\Parameters" /v TimeLong /t REG_DWORD /d 300000 /f
    reg add "HKLM\SYSTEM\CurrentControlSet\services\W64Time\Parameters" /v TimeShort /t REG_DWORD /d 5000 /f


- **ServiceDll**: Menentukan DLL yang mengimplementasikan layanan.
- **Hosts**: Mengatur host dan port (nilai dihapus demi keamanan).
- **Security**: Mengonfigurasi pengaturan keamanan (nilai dihapus demi keamanan).
- **TimeLong**: Pengaturan terkait waktu.
- **TimeShort**: Pengaturan terkait waktu lainnya.

![Screenshot from 2024-06-08 07-18-07](https://github.com/S3N4T0R-0X0/Venomous-Bear-APT/assets/121706460/a1d9236a-12fc-4008-a9a1-0eedb818d0c9)

Ini berarti malware berjalan sebagai layanan yang tersembunyi dalam proses svchost.exe. Fungsi startup `ServiceMain` dari DLL ini tidak melakukan banyak hal selain menjalankan eksekusi.


## Tahap Kedua (Backdoor DLL)

"Di sini, saya telah mengembangkan simulasi backdoor yang digunakan oleh para penyerang dalam serangan sebenarnya."

Pertama, backdoor membaca konfigurasi dari registry dan menyimpannya dalam struktur "result", yang kemudian dialokasikan ke struktur "sConfig".

![Screenshot from 2024-06-08 22-30-51](https://github.com/S3N4T0R-0X0/Venomous-Bear-APT/assets/121706460/b2164d44-bffd-4c9a-9ebe-574c28104eb0)

Backdoor ini mencakup komponen-komponen berikut:

1. **Service Control Handler**: Mendaftarkan pengendali kontrol layanan untuk mengelola status layanan.

2. **Fungsi Utama Malware**: Tempat untuk logika utama dari backdoor.

3. **Pembacaan Konfigurasi**: Menginisialisasi konfigurasi dengan placeholder untuk nilai sebenarnya.

4. **Pengambilan Perintah C2**: Mensimulasikan pengambilan perintah dari server Command and Control (C2).

5. **Pemrosesan Perintah**: Memproses perintah yang diambil (saat ini disimulasikan).

6. **Loop Layanan**: Secara terus-menerus menghubungkan ke server C2 dan memproses perintah, dengan penanganan kesalahan dan pembersihan.

Sesuaikan nilai placeholder dan tambahkan logika sebenarnya untuk operasi backdoor dan pemrosesan perintah C2 sesuai kebutuhan Anda.

![Screenshot from 2024-06-08 22-34-36](https://github.com/S3N4T0R-0X0/Venomous-Bear-APT/assets/121706460/1f3eb42d-b546-4d32-9166-851f0dd00fa6)


## Tahap Ketiga (Pendengar Backdoor)

Di sini, saya mengembangkan skrip pendengar sederhana yang menunggu koneksi masuk dari backdoor saat dijalankan di mesin target.

- **Menerima koneksi masuk**: Ketika klien terhubung, mencetak alamat IP dan port klien.

- **Mengirimkan perintah**: Mengenkripsi perintah sebagai byte dan mengirimkannya melalui soket.

- **Meminta perintah**: Meminta pengguna untuk memasukkan perintah yang akan dikirim ke klien yang terhubung.

- **Melanjutkan membaca hingga tidak ada data lagi yang diterima.**

- **Menerima output dari klien**: Membaca data dalam ukuran blok 4096 byte.

- **Mengakumulasi data** ke dalam variabel output.

![Screenshot from 2024-06-09 10-44-07](https://github.com/S3N4T0R-0X0/Venomous-Bear-APT/assets/121706460/41bfa80d-a18a-4bd3-ad6d-f243bd29bece)

