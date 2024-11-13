# Cozy Bear APT29 Adversary Simulation

Ini adalah simulasi serangan oleh grup Cozy Bear (APT-29) yang menargetkan misi diplomatik. Kampanye dimulai dengan acara yang tampak sah. Pada pertengahan April 2023, seorang diplomat di Kementerian Luar Negeri Polandia mengirimkan brosur resmi yang berisi penawaran penjualan BMW 5-series bekas yang berlokasi di Kyiv. File tersebut berjudul "BMW 5 for sale in Kyiv - 2023.docx." Saya mengandalkan sumber dari Palo Alto untuk menyusun detail simulasi ini: https://unit42.paloaltonetworks.com/cloaked-ursa-phishing/


![1](https://github.com/user-attachments/assets/f46ace53-75c4-41aa-9088-4d5fd97a0573)

## Tahapan Serangan

1. **File DOCX**: File DOCX berisi hyperlink yang mengarahkan untuk mengunduh file HTML tambahan (teknik HTML smuggling).
   
2. **HTML Smuggling**: Penyerang menggunakan HTML smuggling untuk menyembunyikan file ISO.

3. **File LNK**: Ketika file LNK (shortcut) dieksekusi, mereka menjalankan EXE yang sah dan membuka file PNG. Namun, di balik layar, shellcode terenkripsi dibaca ke dalam memori dan didekripsi.

4. **File ISO**: File ISO berisi sejumlah file LNK yang menyamar sebagai gambar. File LNK ini digunakan untuk mengeksekusi payload berbahaya.

5. **DLL Hijacking**: File EXE memuat DLL berbahaya melalui DLL hijacking, memungkinkan penyerang untuk menjalankan kode arbitrer dalam konteks proses yang terinfeksi.

6. **Injeksi Shellcode**: Shellcode yang telah didekripsi kemudian diinjeksikan ke dalam proses Windows yang sedang berjalan, memberi penyerang kemampuan untuk menjalankan kode dengan hak istimewa proses yang terinfeksi.

7. **Eksekusi Payload**: Shellcode mendekripsi dan memuat payload akhir di dalam proses saat ini.

8. **Dropbox C2**: Payload ini berkomunikasi dengan Dropbox dan C2 Primer/Sekunder berdasarkan Microsoft Graph API.

![2](https://github.com/user-attachments/assets/05ffaf6d-8e84-4d1a-9688-ffc84863da4f)


## Tahap Pertama (Teknik Pengiriman)

Pada tahap pertama, penyerang membuat file DOCX yang berisi hyperlink yang mengarah untuk mengunduh HTML (HTML smuggling file). Keunggulan hyperlink adalah bahwa ini tidak muncul dalam teks, dan ini adalah celah yang dieksploitasi oleh penyerang.
![3](https://github.com/user-attachments/assets/25798e65-2dc0-4147-89b3-26cba49d6e44)


HTML Smuggling digunakan untuk menyembunyikan file ISO yang berisi sejumlah file LNK yang menyamar sebagai gambar.
Command line untuk membuat payload Base64 untuk HTML smuggling file:
`base64 payload.iso -w 0` dan saya menambahkan gambar mobil BMW beserta teks konten pesan phishing di dalam file HTML.

![4](https://github.com/user-attachments/assets/a6f3fa36-af57-415b-b35c-2c5e04e36203)


## Tahap Kedua (Teknik Penanaman)

Selanjutnya kita membuat gambar PNG yang berisi gambar mobil BMW, tetapi di latar belakang saat gambar dibuka, malware berjalan di latar belakang. Di tahap ini, saya menggunakan program WinRAR untuk membuka gambar dengan eksekusi Command Line melalui CMD saat membuka gambar dan menggunakan gambar dalam format ikon.

![5](https://github.com/user-attachments/assets/58696a6d-54e8-4183-a5bd-84384bf1c640)


Setelah menggunakan WinRAR untuk file terkompresi ini, kita membuat shortcut file ini dan menempatkannya di file lain dengan gambar asli, lalu mengonversinya menjadi file ISO melalui program PowerISO.

Catatan: File ISO ini yang akan dibuat Base64 dan dimasukkan ke dalam HTML smuggling file sebelum membuat hyperlink dan menempatkannya di file DOCX.

![6](https://github.com/user-attachments/assets/dff8f73d-2382-4756-a623-8ffcbb19b695)


## Tahap Ketiga (Teknik Eksekusi)

Karena saya memasukkan command line dalam menu setup (run after extraction) di Advanced SFX options WinRAR, ketika korban membuka file ISO untuk melihat gambar berkualitas tinggi BMW sesuai pesan phishing yang diterima sebelumnya, payload akan dijalankan bersamaan dengan pembukaan gambar asli BMW.



https://github.com/user-attachments/assets/d4ea918e-559f-4c7d-b773-a93112b7711a



## Tahap Keempat (Eksfiltrasi Data) melalui Channel Dropbox API C2

Penyerang menggunakan Dropbox C2 (Command and Control) API sebagai cara untuk membangun saluran komunikasi antara payload mereka dan server penyerang. Dengan menggunakan Dropbox sebagai server C2, penyerang dapat menyembunyikan aktivitas berbahaya di antara lalu lintas sah Dropbox, sehingga lebih sulit dideteksi oleh tim keamanan. Pertama, kita perlu membuat akun Dropbox dan mengaktifkan izin, seperti pada gambar berikut.

![8](https://github.com/user-attachments/assets/71c181eb-2b30-4333-8072-35b5e91b9e33)


Setelah itu, kita masuk ke menu pengaturan untuk menghasilkan token akses untuk akun Dropbox, yang akan digunakan dalam Dropbox C2.

![9](https://github.com/user-attachments/assets/a07ae0e4-1e49-48b4-819e-c9f5925bffb9)


Script ini mengintegrasikan fungsionalitas API Dropbox untuk memfasilitasi komunikasi antara sistem yang terkompromi dan server yang dikuasai penyerang, menyembunyikan lalu lintas dalam komunikasi Dropbox yang sah, dan menggunakan token akses sebagai input. Pengguna diminta memasukkan kunci AES (16, 24, atau 32 byte) yang kemudian mengenkripsi token menggunakan enkripsi AES dalam mode ECB dan mengkodekannya dengan Base64.


![10](https://github.com/user-attachments/assets/42b130f8-465b-4112-b052-192afcf599df)

Saya menggunakan payload berbasis Python untuk menguji C2 (testing payload.py), jika terdapat masalah dengan koneksi (untuk uji koneksi saja) sebelum menulis payload yang sebenarnya.

## Tahap Kelima (Payload dengan DLL Hijacking) dan Injeksi Shellcode

Payload ini menggunakan Dropbox API untuk mengunggah data, termasuk output perintah ke Dropbox. Dengan menggunakan Dropbox API dan menyediakan token akses, payload menyembunyikan lalu lintasnya di antara lalu lintas sah dari layanan Dropbox, dan jika DLL berbahaya gagal dimuat, itu akan menampilkan pesan peringatan tetapi terus dieksekusi tanpa itu.

![11](https://github.com/user-attachments/assets/c554c893-5f86-4b94-b4d4-182b262dcabc)


1. **DLL Injection**: Payload menggunakan DLL hijacking untuk memuat DLL berbahaya ke dalam ruang alamat dari proses target.

2. **Shellcode Execution**: Setelah injeksi berhasil, DLL berbahaya menjalankan shellcode yang disimpan di dalam fungsi DllMain-nya.

3. **Memory Allocation**: Fungsi VirtualAlloc digunakan untuk mengalokasikan memori dalam proses target, tempat shellcode akan diinjeksikan.

4. **Shellcode Injection**: Shellcode disalin ke dalam area memori yang dialokasikan menggunakan memcpy, menginjeksikannya ke dalam proses.

5. **Privilege Escalation**: Jika proses yang terkompromi berjalan dengan hak istimewa yang tinggi, shellcode yang diinjeksikan mewarisi hak tersebut, memungkinkan penyerang melakukan berbagai operasi, seperti mengakses file sistem atau memodifikasi pengaturan sistem.

![12](https://github.com/user-attachments/assets/453f2996-c1f6-4d88-a018-037a0fc60c9d)


### Kesimpulan

Simulasi ini menunjukkan teknik yang digunakan oleh APT29, termasuk teknik HTML smuggling, DLL hijacking, dan Dropbox API C2, untuk menghindari deteksi. Dengan memahami metodologi ini, kita dapat mempersiapkan langkah-langkah keamanan yang lebih baik untuk mencegah atau mendeteksi aktivitas serupa di masa depan.



https://github.com/user-attachments/assets/83257727-8304-4fe5-a1bd-900427fedd7b


