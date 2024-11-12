# Simulasi Adversari Fancy Bear APT28

Ini adalah simulasi serangan oleh grup Fancy Bear (APT28) yang menargetkan pejabat tinggi pemerintah di Asia Barat dan Eropa Timur. Kampanye serangan ini aktif dari Oktober hingga November 2021. Rantai serangan dimulai dengan eksekusi pengunduh Excel yang dikirim ke korban melalui email, yang memanfaatkan kerentanan eksekusi kode jarak jauh MSHTML (CVE-2021-40444) untuk menjalankan executable berbahaya di memori. Saya mengandalkan informasi dari Trellix untuk detail simulasi ini: [Trellix Blog](https://www.trellix.com/blogs/research/prime-ministers-office-compromised/)

![photo_2024-04-06_23-42-01](https://github.com/S3N4T0R-0X0/APT28-Adversary-Simulation/assets/121706460/bd9e3d64-a453-4aaf-9653-255a0cf4fe68)

Serangan ini mencakup beberapa tahap, termasuk eksploitasi kerentanan CVE-2021-40444 di mana eksekusi jarak jauh dapat diakses melalui file Word. Hal ini dilakukan dengan menyuntikkan DLL ke dalam file Word melalui eksploitasi ini, serta menggunakan server C2 OneDrive untuk mengambil alih kontrol dan eksfiltrasi data, dengan menyembunyikan aktivitas berbahaya di antara lalu lintas sah ke OneDrive.

1. Buat DLL yang mengunduh file melalui base64 untuk mengunduh dua file, yaitu `dfsvc.dll` dan `Stager.dll`.
2. Mengeksploitasi kerentanan zero-day untuk menyuntikkan file DLL ke dalam File Word dan menjalankan DLL saat file Word dibuka.
3. File Word menjalankan payload asli melalui DLLDownloader.dll dan mengunduh dua file `Stager.dll` dan `dfsvc.dll`.
4. Stager mendekripsi payload asli dan menjalankannya yang bertanggung jawab untuk perintah dan kontrol.
5. Eksfiltrasi data melalui OneDrive API C2 Channel, yang mengintegrasikan fungsionalitas API OneDrive untuk memfasilitasi komunikasi antara sistem yang dikompromikan dan server yang dikendalikan oleh penyerang, sehingga berpotensi menyembunyikan lalu lintas dalam komunikasi OneDrive yang sah.
6. Mendapatkan Perintah dan Kontrol melalui payload yang menggunakan API OneDrive untuk mengunggah data, termasuk output perintah ke OneDrive. Payload menghitung checksum CRC32 dari MachineGuid dan memasukkannya dalam komunikasi dengan server untuk identifikasi.

![Screenshot from 2024-04-08 01-28-29](https://github.com/S3N4T0R-0X0/APT28-Adversary-Simulation/assets/121706460/d6d418db-2d9a-4e4c-94fb-74596207d95a)

## Tahap Pertama (Teknik Pengiriman)

Pertama, penyerang membuat executable DLL (DLLDownloader.dll) yang dapat mengunduh dua payload melalui command line untuk membuat payload base64 `base64 dfsvc.dll -w 0` dan `base64 Stager.dll -w 0`, di mana yang pertama adalah `dfsvc.dll` dan yang kedua adalah `Stager.dll`. DLL ini akan digunakan pada tahap berikutnya dengan menyuntikkannya ke dalam file Word melalui kerentanan Zero-day.

![Screenshot from 2024-04-16 21-38-27](https://github.com/S3N4T0R-0X0/APT28-Adversary-Simulation/assets/121706460/d4fdc9a2-5268-42cf-98b0-4e8aff660ac6)

## Tahap Kedua (Teknik Implantasi)

Kedua, penyerang mengeksploitasi kerentanan Zero-day (CVE-2021-40444) [GitHub: CVE-2021-40444](https://github.com/lockedbyte/CVE-2021-40444/) yang bekerja dengan menyuntikkan file DLL ke dalam Microsoft Word. Saat file dibuka, payload DLL dieksekusi, yang bertanggung jawab untuk mengunduh dua payload lainnya (`dfsvc.dll` dan `Stager.dll`).

![Screenshot from 2024-04-13 01-09-30](https://github.com/S3N4T0R-0X0/APT28-Adversary-Simulation/assets/121706460/e03bfa09-ed13-4ddc-bf41-64d97187099b)

Ketika korban membuka dokumen Office berbahaya menggunakan Microsoft Office, aplikasi memproses konten dokumen, termasuk objek yang disematkan. Cacat dalam komponen MSHTML dipicu selama proses ini, memungkinkan kode berbahaya penyerang untuk dieksekusi dalam konteks aplikasi Office.

## Tahap Ketiga (Teknik Eksekusi)

Sekarang, saat file Word dibuka, file tersebut menjalankan eksekusi untuk DLL Downloader dan mengunduh dua file `dfsvc.dll` dan `Stager.dll` melalui kerentanan CVE-2021-40444.

![Screenshot from 2024-04-16 17-21-17](https://github.com/S3N4T0R-0X0/APT28-Adversary-Simulation/assets/121706460/b496b5a9-28e9-49f1-a5c7-8324913cbf2f)

Setelah itu, stager mendekripsi payload menggunakan fungsi `Decrypt-Payload` (perlu menerapkan algoritma dekripsi) dan kemudian mengeksekusi payload menggunakan fungsi `Execute-Payload`. Dalam simulasi ini, saya membuat build yang menjalankan eksekusi langsung tanpa perlu script stager, dan dapat disesuaikan agar stager mengeksekusi payload sebenarnya.

![Screenshot from 2024-04-16 17-59-42](https://github.com/S3N4T0R-0X0/APT28-Adversary-Simulation/assets/121706460/30729f3f-d294-4ccc-82c3-7f1821f792df)

## Tahap Keempat (Eksfiltrasi Data) melalui OneDrive API C2 Channel

Penyerang menggunakan OneDrive C2 API sebagai sarana untuk menjalin saluran komunikasi antara payload mereka dan server yang dikendalikan penyerang. Dengan menggunakan OneDrive sebagai server C2, penyerang dapat menyembunyikan aktivitas berbahaya di antara lalu lintas sah ke OneDrive, membuatnya lebih sulit bagi tim keamanan untuk mendeteksi ancaman. Pertama, kami perlu membuat akun Microsoft Azure dan mengaktifkan izin-izinnya.

Kami akan menggunakan Application (client) ID sebagai input yang diperlukan oleh server C2.

![photo_2024-04-14_16-24-06](https://github.com/S3N4T0R-0X0/APT28-Adversary-Simulation/assets/121706460/6e73395a-2221-411b-ab4a-e6c23f2b2897)

Setelah itu, kami akan pergi ke menu Certificates & secrets untuk menghasilkan Secret ID untuk akun Microsoft Azure, dan ini yang akan kami gunakan di OneDrive C2.

![photo_2024-04-14_16-24-14](https://github.com/S3N4T0R-0X0/APT28-Adversary-Simulation/assets/121706460/fec5b59d-57ed-47f4-b640-d06782d8c16b)

Untuk simulasi serangan ini, saya tidak menggunakan PowerShell Empire untuk menghindari deteksi, melainkan melakukan kustomisasi pada server C2 OneDrive. Script ini mengintegrasikan fungsionalitas API OneDrive untuk memfasilitasi komunikasi antara sistem yang dikompromikan dan server yang dikendalikan penyerang, serta menggunakan enkripsi AES untuk mengamankan koneksi seperti yang dilakukan server PowerShell Empire dalam serangan sebenarnya.

![photo_2024-04-14_02-55-02](https://github.com/S3N4T0R-0X0/APT28-Adversary-Simulation/assets/121706460/0e4a4178-053b-493d-90b2-f0988d80f5da)

## Tahap Kelima (Payload dengan Permintaan API OneDrive)

Payload ini membentuk komunikasi tersembunyi melalui socket ke server jarak jauh, menyamarkan lalu lintas dalam permintaan API OneDrive. Payload ini mengidentifikasi mesin dengan menggunakan checksum CRC32 dari MachineGuid. Perintah dijalankan secara lokal, dengan output dikirim kembali ke server atau diunggah ke OneDrive. Konfigurasi dinamisnya memungkinkan kontrol jarak jauh yang fleksibel dan eksfiltrasi data yang tersembunyi.

![Screenshot from 2024-04-14 16-59-47](https://github.com/S3N4T0R-0X0/APT28-Adversary-Simulation/assets/121706460/b784cfdd-e83e-41b3-857b-23e56396312d)

1. **Komunikasi Tersembunyi**: Payload memulai koneksi socket ke alamat IP dan port yang ditentukan.
  
2. **Mekanisme Identifikasi**: Mengambil MachineGuid dari registri Windows dan menghitung checksum CRC32-nya.

![171351508026027259](https://github.com/S3N4T0R-0X0/APT28-Adversary-Simulation/assets/121706460/ba5979bc-eb9b-4e98-b74a-002e6846ff36)

3. **Eksekusi Perintah**: Payload memasuki loop untuk menerima perintah dari server jarak jauh atau OneDrive.

4. **Eksfiltrasi Data**: Setelah eksekusi, payload menangkap output dan mengirimkannya kembali ke server atau mengunggahnya ke OneDrive.

5. **Komunikasi Tersembunyi**: Dengan memanfaatkan API OneDrive, lalu lintas jaringan disamarkan dengan lalu lintas sah OneDrive.

6. **Konfigurasi Dinamis**: Perilaku dikonfigurasi dengan menentukan alamat IP, port, dan opsional token akses untuk OneDrive.

![Screenshot from 2024-04-14 22-43-29](https://github.com/S3N4T0R-0X0/APT28-Adversary-Simulation/assets/121706460/b2eda097-d7f7-48ab-823d-f720badf69f1)

## Hasil Akhir: Payload Terhubung ke Server C2 OneDrive

Langkah terakhir dalam proses ini melibatkan eksekusi payload akhir. Setelah didekripsi dan dimuat ke dalam proses saat ini, payload akhir dirancang untuk berkomunikasi dengan server C2 berbasis API OneDrive.

https://github.com/S3N4T0R-0X0/APT28-Adversary-Simulation/assets/121706460/becef683-c49b-40d5-9047-d8e8c6303eaa

