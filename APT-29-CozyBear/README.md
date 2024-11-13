# Cozy Bear APT29 Adversary Simulation

Ini adalah simulasi serangan oleh grup Cozy Bear (APT-29) yang menargetkan misi diplomatik. Kampanye dimulai dengan acara yang tampak sah. Pada pertengahan April 2023, seorang diplomat di Kementerian Luar Negeri Polandia mengirimkan brosur resmi yang berisi penawaran penjualan BMW 5-series bekas yang berlokasi di Kyiv. File tersebut berjudul "BMW 5 for sale in Kyiv - 2023.docx." Saya mengandalkan sumber dari Palo Alto untuk menyusun detail simulasi ini: https://unit42.paloaltonetworks.com/cloaked-ursa-phishing/

![photo_2024-04-12_01-35-08](https://github.com/S3N4T0R-0X0/APT29-Adversary-Simulation/assets/121706460/891d43ef-b749-4c08-ab60-df2df85e620d)

## Tahapan Serangan

1. **File DOCX**: File DOCX berisi hyperlink yang mengarahkan untuk mengunduh file HTML tambahan (teknik HTML smuggling).
   
2. **HTML Smuggling**: Penyerang menggunakan HTML smuggling untuk menyembunyikan file ISO.

3. **File LNK**: Ketika file LNK (shortcut) dieksekusi, mereka menjalankan EXE yang sah dan membuka file PNG. Namun, di balik layar, shellcode terenkripsi dibaca ke dalam memori dan didekripsi.

4. **File ISO**: File ISO berisi sejumlah file LNK yang menyamar sebagai gambar. File LNK ini digunakan untuk mengeksekusi payload berbahaya.

5. **DLL Hijacking**: File EXE memuat DLL berbahaya melalui DLL hijacking, memungkinkan penyerang untuk menjalankan kode arbitrer dalam konteks proses yang terinfeksi.

6. **Injeksi Shellcode**: Shellcode yang telah didekripsi kemudian diinjeksikan ke dalam proses Windows yang sedang berjalan, memberi penyerang kemampuan untuk menjalankan kode dengan hak istimewa proses yang terinfeksi.

7. **Eksekusi Payload**: Shellcode mendekripsi dan memuat payload akhir di dalam proses saat ini.

8. **Dropbox C2**: Payload ini berkomunikasi dengan Dropbox dan C2 Primer/Sekunder berdasarkan Microsoft Graph API.

![Screenshot from 2024-03-11 15-43-36](https://github.com/S3N4T0R-0X0/APT29-Adversary-Simulation/assets/121706460/ff510ffd-3481-4978-bedc-d30629d65307)

## Tahap Pertama (Teknik Pengiriman)

Pada tahap pertama, penyerang membuat file DOCX yang berisi hyperlink yang mengarah untuk mengunduh HTML (HTML smuggling file). Keunggulan hyperlink adalah bahwa ini tidak muncul dalam teks, dan ini adalah celah yang dieksploitasi oleh penyerang.

![Screenshot from 2024-03-01 19-18-51](https://github.com/S3N4T0R-0X0/APT29-Adversary-Simulation/assets/121706460/bb984b9c-5367-4fb2-9efc-3be7c098ec46)

HTML Smuggling digunakan untuk menyembunyikan file ISO yang berisi sejumlah file LNK yang menyamar sebagai gambar.
Command line untuk membuat payload Base64 untuk HTML smuggling file:
`base64 payload.iso -w 0` dan saya menambahkan gambar mobil BMW beserta teks konten pesan phishing di dalam file HTML.

![Screenshot from 2024-03-01 19-39-42](https://github.com/S3N4T0R-0X0/APT29-Adversary-Simulation/assets/121706460/8e76572b-5d72-4d87-9cf9-4c7bf002c801)

## Tahap Kedua (Teknik Penanaman)

Selanjutnya kita membuat gambar PNG yang berisi gambar mobil BMW, tetapi di latar belakang saat gambar dibuka, malware berjalan di latar belakang. Di tahap ini, saya menggunakan program WinRAR untuk membuka gambar dengan eksekusi Command Line melalui CMD saat membuka gambar dan menggunakan gambar dalam format ikon.

![20240302_194641](https://github.com/S3N4T0R-0X0/APT29-Adversary-Simulation/assets/121706460/b3b7872e-1bf9-4637-a13f-ba720c113276)

Setelah menggunakan WinRAR untuk file terkompresi ini, kita membuat shortcut file ini dan menempatkannya di file lain dengan gambar asli, lalu mengonversinya menjadi file ISO melalui program PowerISO.

Catatan: File ISO ini yang akan dibuat Base64 dan dimasukkan ke dalam HTML smuggling file sebelum membuat hyperlink dan menempatkannya di file DOCX.

![Screenshot from 2024-03-02 15-39-55](https://github.com/S3N4T0R-0X0/APT29-Adversary-Simulation/assets/121706460/40fef200-416c-4de4-8999-f154a01b22dd)

## Tahap Ketiga (Teknik Eksekusi)

Karena saya memasukkan command line dalam menu setup (run after extraction) di Advanced SFX options WinRAR, ketika korban membuka file ISO untuk melihat gambar berkualitas tinggi BMW sesuai pesan phishing yang diterima sebelumnya, payload akan dijalankan bersamaan dengan pembukaan gambar asli BMW.

https://github.com/S3N4T0R-0X0/APT29-Adversary-Simulation/assets/121706460/343e37ce-28e0-4a52-8a68-673bfcc68ffe

## Tahap Keempat (Eksfiltrasi Data) melalui Channel Dropbox API C2

Penyerang menggunakan Dropbox C2 (Command and Control) API sebagai cara untuk membangun saluran komunikasi antara payload mereka dan server penyerang. Dengan menggunakan Dropbox sebagai server C2, penyerang dapat menyembunyikan aktivitas berbahaya di antara lalu lintas sah Dropbox, sehingga lebih sulit dideteksi oleh tim keamanan. Pertama, kita perlu membuat akun Dropbox dan mengaktifkan izin, seperti pada gambar berikut.

![Screenshot from 2024-03-12 16-10-13](https://github.com/S3N4T0R-0X0/APT29-Adversary-Simulation/assets/121706460/518a643a-f8bc-455c-acdd-a6ed6fe8735a)

Setelah itu, kita masuk ke menu pengaturan untuk menghasilkan token akses untuk akun Dropbox, yang akan digunakan dalam Dropbox C2.

![photo_2024-03-12_16-22-54](https://github.com/S3N4T0R-0X0/APT29-Adversary-Simulation/assets/121706460/00e41c7e-b2ac-4805-b1a9-77d00671ebf8)

Script ini mengintegrasikan fungsionalitas API Dropbox untuk memfasilitasi komunikasi antara sistem yang terkompromi dan server yang dikuasai penyerang, menyembunyikan lalu lintas dalam komunikasi Dropbox yang sah, dan menggunakan token akses sebagai input. Pengguna diminta memasukkan kunci AES (16, 24, atau 32 byte) yang kemudian mengenkripsi token menggunakan enkripsi AES dalam mode ECB dan mengkodekannya dengan Base64.

![171053992557140444](https://github.com/S3N4T0R-0X0/APT29-Adversary-Simulation/assets/121706460/15fdca80-68cb-41ac-9eb5-b56ded6e552e)

Saya menggunakan payload berbasis Python untuk menguji C2 (testing payload.py), jika terdapat masalah dengan koneksi (untuk uji koneksi saja) sebelum menulis payload yang sebenarnya.

## Tahap Kelima (Payload dengan DLL Hijacking) dan Injeksi Shellcode

Payload ini menggunakan Dropbox API untuk mengunggah data, termasuk output perintah ke Dropbox. Dengan menggunakan Dropbox API dan menyediakan token akses, payload menyembunyikan lalu lintasnya di antara lalu lintas sah dari layanan Dropbox, dan jika DLL berbahaya gagal dimuat, itu akan menampilkan pesan peringatan tetapi terus dieksekusi tanpa itu.

![Screenshot from 2024-03-23 15-17-27](https://github.com/S3N4T0R-0X0/APT29-Adversary-Simulation/assets/121706460/7144331f-5635-49f7-b024-5152cb06cc03)

1. **DLL Injection**: Payload menggunakan DLL hijacking untuk memuat DLL berbahaya ke dalam ruang alamat dari proses target.

2. **Shellcode Execution**: Setelah injeksi berhasil, DLL berbahaya menjalankan shellcode yang disimpan di dalam fungsi DllMain-nya.

3. **Memory Allocation**: Fungsi VirtualAlloc digunakan untuk mengalokasikan memori dalam proses target, tempat shellcode akan diinjeksikan.

4. **Shellcode Injection**: Shellcode disalin ke dalam area memori yang dialokasikan menggunakan memcpy, menginjeksikannya ke dalam proses.

5. **Privilege Escalation**: Jika proses yang terkompromi berjalan dengan hak istimewa yang tinggi, shellcode yang diinjeksikan mewarisi hak tersebut, memungkinkan penyerang melakukan berbagai operasi, seperti mengakses file sistem atau memodifikasi pengaturan sistem.

![Screenshot from 2024-03-23 15-16-20](https://github.com/S3N4T0R-0X0/APT29-Adversary-Simulation/assets/121706460/29858785-5fc5-446c-a1d0-d0b1bb1e58d7)

### Kesimpulan

Simulasi ini menunjukkan teknik yang digunakan oleh APT29, termasuk teknik HTML smuggling, DLL hijacking, dan Dropbox API C2, untuk menghindari deteksi. Dengan memahami metodologi ini, kita dapat mempersiapkan langkah-langkah keamanan yang lebih baik untuk mencegah atau mendeteksi aktivitas serupa di masa depan.

https://github.com/S3N4T0R-0X0/APT29-Adversary-Simulation/assets/121706460/c5b7b826-72a1-459e-9f19-6e34bd79aeab
