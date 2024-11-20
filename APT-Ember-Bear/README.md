# Ember Bear APT Adversary Simulation

Simulasi ini adalah serangan oleh grup APT (Ember Bear) yang menargetkan organisasi energi di Ukraina. Kampanye serangan ini aktif pada April 2021. Rantai serangan dimulai dengan email spear phishing yang dikirimkan kepada seorang karyawan organisasi, menggunakan tema rekayasa sosial yang menyarankan bahwa individu tersebut telah melakukan kejahatan. Email tersebut memiliki lampiran dokumen Word yang berisi file JavaScript berbahaya yang akan mengunduh dan menginstal payload yang dikenal sebagai SaintBot (downloader) dan OutSteel (pencuri dokumen).

OutSteel adalah alat pencuri dokumen sederhana. Alat ini mencari dokumen yang berpotensi sensitif berdasarkan jenis file-nya dan mengunggah file tersebut ke server jarak jauh. Penggunaan OutSteel menunjukkan bahwa tujuan utama grup ini adalah pengumpulan data pada organisasi pemerintah dan perusahaan yang terlibat dalam infrastruktur penting. SaintBot adalah alat pengunduh yang memungkinkan pelaku ancaman mengunduh dan menjalankan alat tambahan di sistem yang terinfeksi. SaintBot memberikan akses persisten kepada pelaku ancaman ke sistem sambil memberikan kemampuan untuk memperluas operasi mereka. Saya mengandalkan informasi dari Palo Alto untuk membuat simulasi ini: https://unit42.paloaltonetworks.com/ukraine-targeted-outsteel-saintbot/

![1](https://github.com/user-attachments/assets/b18d0412-708b-4325-b0c3-bf615997c553)


Serangan ini mencakup beberapa tahap, termasuk tautan ke arsip Zip yang berisi pintasan berbahaya (LNK) dalam email spear phishing, serta lampiran berupa dokumen PDF, dokumen Word, file JavaScript, dan file Eksekutabel Control Panel (CPL). Bahkan dokumen Word yang dilampirkan pada email menggunakan berbagai teknik, termasuk macro berbahaya, JavaScript yang disematkan, dan eksploitasi CVE-2017-11882 untuk menginstal payload ke dalam sistem. Dengan pengecualian file CPL, sebagian besar mekanisme pengiriman mengandalkan skrip PowerShell untuk mengunduh dan menjalankan kode dari server jarak jauh.

1. Membuat Dokumen Word: Menulis dokumen Word (.docx) yang berisi eksploitasi CVE-2017-11882 untuk menginstal payload ke dalam sistem.

2. CVE-2017-11882: Eksploitasi ini memungkinkan pelaku ancaman menjalankan kode arbitrer dalam konteks pengguna saat ini dengan tidak menangani objek di memori dengan benar.

3. Eksfiltrasi data: melalui saluran API Discord C2. Ini mengintegrasikan fungsionalitas API Discord untuk memfasilitasi komunikasi antara sistem yang dikompromikan dan server yang dikendalikan pelaku ancaman, sehingga berpotensi menyembunyikan lalu lintas dalam komunikasi Discord yang sah.

4. SaintBot: adalah loader payload. Ini memiliki kemampuan untuk mengunduh payload lebih lanjut sesuai permintaan pelaku ancaman.

5. Pelaku ancaman menggunakan file .BAT untuk menonaktifkan fungsi Windows Defender. Ini dilakukan dengan menjalankan beberapa perintah melalui CMD yang memodifikasi kunci registry dan menonaktifkan tugas terjadwal Windows Defender.

6. OutSteel: adalah pengunggah file dan pencuri dokumen yang dikembangkan dengan bahasa skrip.

Contoh beberapa file PDF dan dokumen Word yang digunakan dalam serangan ini:

![2](https://github.com/user-attachments/assets/d6268458-3b55-45f2-a2bc-bfd17101dc81)


## Tahap Pertama (teknik pengiriman)

Pada awalnya, saya akan membuat file Word yang akan saya gunakan untuk injeksi kerentanan yang digunakan pelaku ancaman dalam serangan sebenarnya untuk menginstal payload ke dalam sistem.

April 2021: Email spear phishing bertema Bitcoin yang menargetkan organisasi pemerintah Ukraina.

![3](https://github.com/user-attachments/assets/c8469a14-7366-4681-bdd3-635e6413ee9b)



## Tahap Kedua (eksploitasi Kerentanan Memori Microsoft Office CVE-2017-11882)

Pada tahap kedua, pelaku ancaman mengeksploitasi kerentanan Zero-day (CVE-2017-11882), yaitu kerentanan di Microsoft Office, khususnya Microsoft Office 2007 Service Pack 3, Microsoft Office 2010 Service Pack 2, Microsoft Office 2013 Service Pack 1, dan Microsoft Office 2016. Kerentanan ini diklasifikasikan sebagai masalah korupsi memori yang terjadi karena kesalahan penanganan objek di memori.

Repositori eksploitasi: https://github.com/0x09AL/CVE-2017-11882-metasploit?tab=readme-ov-file

Kerentanan ini memungkinkan pelaku ancaman menjalankan kode arbitrer dalam konteks pengguna saat ini. Saya kemudian menyertakan file Word dalam email phishing, termasuk tautan ke file Zip yang berisi pintasan berbahaya (LNK).


![4](https://github.com/user-attachments/assets/7ee006a3-4014-433f-a45a-a2f2db43743d)



`sudo cp cve_2017_11882.rb /usr/share/metasploit-framework/modules/exploits/windows/fileformat`

`sudo updatedb`

`msf6 > use exploit/windows/fileformat/office_ms17_11882`


## Tahap Ketiga (Eksfiltrasi Data) melalui Discord API C2 Channel

Penyerang menggunakan Discord API C2 (Command and Control) sebagai sarana untuk membangun saluran komunikasi antara payload mereka dan server yang dikendalikan oleh penyerang. Dengan menggunakan Discord sebagai server C2, penyerang dapat menyembunyikan aktivitas berbahaya mereka di antara lalu lintas sah ke Discord, sehingga lebih sulit bagi tim keamanan untuk mendeteksi ancaman tersebut.

![5](https://github.com/user-attachments/assets/5ad49337-5f7c-40a2-a21c-20fca2a114e7)


Pertama, saya perlu membuat akun Discord dan mengaktifkan izinnya, seperti yang ditunjukkan dalam gambar berikut.

1. Buat Aplikasi Discord.

<img width="1679" alt="image-20231221113019757" src="https://github.com/S3N4T0R-0X0/Ember-Bear-APT/assets/121706460/2ac6a61e-719d-49eb-a610-f75808dd5e1a">

2. Konfigurasikan Aplikasi Discord.

<img width="1679" alt="image-20231221113340790" src="https://github.com/S3N4T0R-0X0/Ember-Bear-APT/assets/121706460/f88c0cc8-1abd-462e-bdff-0adadc85914a">

3. Pergi ke "Bot", temukan "Privileged Gateway Intents", aktifkan semua tiga "Intents", dan simpan.

<img width="1679" alt="image-20231221113617087" src="https://github.com/S3N4T0R-0X0/Ember-Bear-APT/assets/121706460/cbb3858f-f51c-454f-8458-6478a56b92c6">

Skrip ini mengintegrasikan fungsionalitas Discord API untuk memfasilitasi komunikasi antara sistem yang dikompromikan dan server yang dikendalikan oleh penyerang, sehingga berpotensi menyembunyikan lalu lintas dalam komunikasi Discord yang sah. Skrip ini juga memeriksa apakah token bot Discord dan ID channel disediakan. Jika ya, maka fungsionalitas bot Discord dimulai; jika tidak, skrip melanjutkan hanya dengan IP dan port. Dengan cara ini, skrip dapat melanjutkan koneksi tanpa detail Discord jika tidak dimasukkan.

![9](https://github.com/user-attachments/assets/339a3da9-fa35-4a8f-ba38-54130ba21819)


## Tahap Keempat (SaintBot Payload Loader)

SaintBot adalah malware loader yang baru ditemukan, didokumentasikan pada April 2021 oleh MalwareBytes. Ini memiliki kemampuan untuk mengunduh payload lebih lanjut sesuai permintaan aktor ancaman, mengeksekusi payload tersebut melalui beberapa cara berbeda, seperti menyuntikkan ke dalam proses yang dijalankan atau memuat ke dalam memori lokal. Ia juga dapat memperbarui dirinya sendiri di disk – dan menghapus jejak keberadaannya – sesuai kebutuhan. SHA-256: e8207e8c31a8613112223d126d4f12e7a5f8caf4acaaf40834302ce49f37cc9c

1. Pemeriksaan Lokasi: Fungsi `IsSupportedLocale` memeriksa apakah lokasi sistem cocok dengan lokasi tertentu.

2. Mengunduh Payload: Fungsi `DownloadPayload` mengunduh file dari URL tertentu dan menyimpannya ke path file tertentu.

![10](https://github.com/user-attachments/assets/a6e48cdf-9d0e-4005-a49c-08fbc6747d9e)


3. Menyuntikkan ke Proses: Fungsi `InjectIntoProcess` menyuntikkan DLL ke dalam proses yang sedang berjalan berdasarkan namanya.

4. Penghapusan Diri: Fungsi `SelfDelete` menghapus file executable setelah eksekusinya selesai.

![11](https://github.com/user-attachments/assets/016ac78a-5d1c-4b1a-aac6-0d7524372a88)


## Tahap Kelima (Menonaktifkan Windows Defender)

File batch ini digunakan untuk menonaktifkan fungsi Windows Defender. Ini dilakukan dengan mengeksekusi beberapa perintah melalui CMD yang memodifikasi kunci registry dan menonaktifkan tugas terjadwal Windows Defender.

![12](https://github.com/user-attachments/assets/4566b621-398b-47f3-9a6f-4930bdfd97a0)


## Tahap Keenam (OutSteel Stealer)

OutSteel adalah pengunggah file dan pencuri dokumen yang dikembangkan dengan bahasa skrip AutoIT. Ini dijalankan bersama dengan biner lainnya. Alat ini mulai dengan memindai disk lokal untuk mencari file dengan ekstensi tertentu, sebelum mengunggah file tersebut ke server command and control (C2) yang telah dikodekan secara keras. Saya mensimulasikan Infostealer ini melalui Skrip PowerShell.

![13](https://github.com/user-attachments/assets/19584a97-25f5-4da9-9785-b48be1fe41ff)


