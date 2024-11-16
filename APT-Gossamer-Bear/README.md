# Simulasi Serangan APT Gossamer Bear

Ini adalah simulasi serangan oleh grup APT (Gossamer Bear) yang menargetkan institusi logistik dan dukungan pertahanan untuk Ukraina. Kampanye serangan ini aktif sejak April 2023. Rantai serangan dimulai dengan mengirimkan pesan dengan lampiran file PDF atau tautan ke file PDF yang dihosting di platform penyimpanan cloud. File PDF tersebut tidak dapat dibaca, dengan tombol mencolok yang mengklaim dapat mengaktifkan konten. Menekan tombol dalam file PDF akan membuka tautan yang tertanam di kode file PDF menggunakan browser default. Ini adalah awal dari rantai pengalihan. Target kemungkinan akan melihat halaman berjudul “Docs” di halaman awal yang terbuka dan mungkin diminta untuk menyelesaikan CAPTCHA sebelum melanjutkan. Sesi penjelajahan akan berakhir dengan menampilkan layar masuk ke akun di mana email phishing diterima, dengan email target sudah terisi di bidang nama pengguna. Saya mengandalkan Microsoft untuk mendapatkan detail dalam membuat simulasi ini:  
[Microsoft Blog](https://www.microsoft.com/en-us/security/blog/2023/12/07/star-blizzard-increases-sophistication-and-evasion-in-ongoing-attacks/)

![1](https://github.com/user-attachments/assets/66b1c71d-f1a3-43cd-bbba-9b38386f873e)


Serangan ini melibatkan beberapa tahap, termasuk membuat file PDF dengan tautan hiper di dalamnya. File PDF tersebut tidak dapat dibaca dan memiliki tombol mencolok yang dimaksudkan untuk mengaktifkan konten. Menekan tombol dalam file PDF akan membuka tautan ke halaman palsu yang mencuri kredensial target. Selain itu, dari file PDF ini, saya juga membuat mekanisme untuk mendapatkan Command and Control (C2).

## Tahap Serangan

1. **File PDF**: File PDF yang dibuat menyertakan tautan hiper yang mengarah ke halaman palsu yang mencuri kredensial.

2. **HTML Smuggling**: Teknik ini digunakan untuk membuka URL halaman phishing kredensial dan juga menginstal payload.

3. **Tombol PDF**: Ketika tombol mencolok dalam file PDF diklik, ini meluncurkan file HTML smuggling di server Apache yang berisi payload dalam base64 dan tautan phishing.

4. **Eksfiltrasi Data**: Melalui saluran C2 API Google Drive. Ini mengintegrasikan fungsionalitas API Google Drive untuk memfasilitasi komunikasi antara sistem yang terinfeksi dan server yang dikendalikan penyerang, sehingga menyembunyikan lalu lintas dalam komunikasi Google Drive yang sah.

5. **Payload Reverse Shell**: Membuat koneksi TCP ke server C2 dan mendengarkan perintah untuk dijalankan di mesin target.

6. **Eksekusi Akhir**: Payload terakhir diunduh melalui file HTML yang dikaburkan dengan encoding base64, dan tautan phishing dibuka.

![2](https://github.com/user-attachments/assets/630f06bd-0257-4ce6-b3e9-3417055eb481)


### Tahap Pertama: Teknik Pengiriman

Penyerang membuat file PDF yang menyertakan tautan hiper yang mengarah ke halaman palsu untuk mencuri kredensial. Keuntungan dari tautan hiper ini adalah tidak terlihat dalam teks, yang dieksploitasi oleh penyerang.

![3](https://github.com/user-attachments/assets/458a6352-3377-488c-9a6f-cd14d49842e3)


**HTML Smuggling** digunakan untuk membuka URL halaman phishing kredensial dan juga menginstal payload untuk mendapatkan kontrol C2. Setelah itu, saya menempatkan file HTML di server Apache, mengambil localhost, dan menggunakannya sebagai tautan hiper pada tombol mencolok dalam file PDF.

![4](https://github.com/user-attachments/assets/72984ec7-47d6-4e5c-b7a5-b021060c545f)


### Tahap Kedua: Teknik Penyisipan

Saya menempatkan tautan phishing di dalam file HTML, selain payload yang dienkode dalam base64. Dalam simulasi ini, saya menggunakan alat PyPhisher.

PyPhisher: [https://github.com/KasRoudra2/PyPhisher.git](https://github.com/KasRoudra2/PyPhisher.git)

`base64 payload.exe`

![5](https://github.com/user-attachments/assets/97e128f2-e5e9-49a7-a83b-60c049e88d71)


Setelah itu, saya mengaburkan file HTML setelah menambahkan tautan phishing dan payload ke dalamnya sebelum menempatkannya di server Apache.

Saya menggunakan alat WMTips untuk mengaburkan file HTML: [HTML Obfuscator](https://www.wmtips.com/tools/html-obfuscator/#google_vignette)

![6](https://github.com/user-attachments/assets/afd638ec-d1dc-48bf-b72c-e7a70719d678)


### Tahap Ketiga: Teknik Eksekusi

Ketika tombol mencolok dalam file PDF diklik, file HTML smuggling di server Apache akan dijalankan, yang berisi payload dalam base64 dan tautan phishing.

![7](https://github.com/user-attachments/assets/4962e5ce-af04-4009-a7d7-56e37a8457b3)


### Tahap Keempat: Eksfiltrasi Data melalui Saluran C2 API Google Drive

Dalam serangan sebenarnya, penyerang tidak menggunakan server C2 nyata atau payload, melainkan hanya menggunakan spear phishing. Namun, di sini saya memanfaatkan file HTML yang lebih besar untuk mengunduh payload dan membuka URL berbahaya.

Langkah-langkah:
1. Masuk ke Google Cloud Platform.
2. Buat proyek di dasbor Google Cloud Platform.
3. Aktifkan Google Drive API.
4. Buat kunci API Google Drive.

![8](https://github.com/user-attachments/assets/b6608e44-127f-4cd7-9785-d22248eb9d3b)


Dengan menggunakan API Google Drive sebagai server C2, saya dapat menyembunyikan aktivitas berbahaya di antara lalu lintas yang sah ke Google Drive, membuat ancaman ini sulit terdeteksi oleh tim keamanan.

![9](https://github.com/user-attachments/assets/f0c54df4-c139-428f-ac3b-a775163dc10c)


### Tahap Kelima: Payload dengan Reverse Shell

Payload ini adalah reverse shell sederhana yang ditulis dalam Rust, yang membuat koneksi TCP ke server C2 dan mendengarkan perintah untuk dieksekusi pada mesin yang terinfeksi. Payload akan terus berjalan hingga koneksi ditutup oleh server C2 atau terjadi kesalahan.

![10](https://github.com/user-attachments/assets/78c3c5ed-f7ba-4b91-8279-43b88e1ae0b3)


### Hasil Akhir: Payload Terhubung ke Server C2 Google Drive

Langkah terakhir dalam proses ini melibatkan eksekusi payload akhir, yang diunduh melalui file HTML yang telah dikaburkan dengan encoding base64 dan tautan phishing dibuka.



https://github.com/user-attachments/assets/5af6eef0-74e9-4356-afa5-0f832aa13e18


