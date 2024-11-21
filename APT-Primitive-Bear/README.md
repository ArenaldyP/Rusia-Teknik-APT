# Simulasi Serangan APT Primitive Bear

Ini adalah simulasi serangan oleh kelompok APT (Primitive Bear) yang menargetkan Layanan Migrasi Negara Ukraina. Kampanye serangan ini aktif dari bulan Desember hingga Juni 2021. Rantai serangan dimulai dengan dokumen Word yang dikirimkan ke korban melalui email, kemudian payload VBS digunakan untuk mendapatkan kendali dan komunikasi (Command and Control). Sebelum menempatkan payload atau menyuntikkannya ke dalam file Word, dilakukan obfuscation pada payload untuk menghindari deteksi, kemudian payload tersebut disuntikkan melalui macro ke dalam dokumen Word. Selanjutnya, saya membuat arsip SFX dan memasukkan file Word dengan payload di dalamnya untuk mendapatkan kendali dan komunikasi. Arsip SFX ini digunakan untuk melakukan serangan spear phishing, dan saya memperoleh kendali dengan membuka file Word. Saya mengandalkan Palo Alto Networks untuk mendapatkan detail serangan ini untuk membuat simulasi ini: [Palo Alto Networks Update](https://unit42.paloaltonetworks.com/gamaredon-primitive-bear-ukraine-update-2021/)

![imageedit_2_9352621513](https://github.com/S3N4T0R-0X0/Primitive-Bear-APT/assets/121706460/a715b1e5-5d3f-48af-a749-7651cb857341)

Serangan ini melibatkan beberapa tahap, termasuk membuat file SFX dengan file Word di dalamnya. File Word ini berisi skrip VBS yang bertanggung jawab untuk command and control dan membuat obfuscation pada skrip VBS payload sebelum menempatkannya ke dalam file Word. Payload ini dikirim melalui serangan spear phishing dan melakukan komunikasi jarak jauh dengan memanfaatkan enkripsi DES untuk transmisi data yang aman antara server penyerang dan target.

1. **Membuat Dokumen Word**: Menulis dokumen Word (.doc atau .docx) yang berisi macro dengan payload VBS yang sudah di-obfuscate. Macro ini harus dirancang untuk mengeksekusi payload saat dokumen dibuka.

2. **Membuat Payload VBScript**: Membuat payload VBS yang dirancang untuk membangun koneksi balik (reverse connection) ke server Command and Control (C2).

3. **Melakukan Obfuscation pada Payload VBS**: Melakukan obfuscation pada payload VBS agar lebih sulit dideteksi oleh perangkat lunak antivirus atau solusi keamanan lainnya.

4. **Membuat Arsip Self-Extracting dengan WinRAR**: Menggunakan WinRAR untuk membuat arsip self-extracting (SFX).
Menambahkan dokumen Word yang berisi macro dan payload VBS yang di-obfuscate ke dalam arsip.

5. **Menempatkan Payload VBS yang di-obfuscate dan file Word di dalam arsip SFX untuk dikirim ke target**.

6. **Hasil Akhir**: Melakukan komunikasi jarak jauh dengan memanfaatkan enkripsi DES untuk transmisi data yang aman antara server penyerang dan target.

![word-image-4](https://github.com/S3N4T0R-0X0/Primitive-Bear-APT/assets/121706460/9e4ac08a-9ae5-4b39-ad41-ed9d82cc65b6)

## Tahap pertama (teknik pengiriman)

Saya mulai dengan menyusun email phishing dalam dokumen Word untuk serangan yang akan datang. Setelah itu, sebelum membuat payload, yang akan terdiri dari skrip VBS yang disuntikkan ke dalam macro, saya akan mengenkapsulasinya dalam file SFX. Serangan ini menargetkan Departemen Imigrasi Ukraina, dengan surat phishing yang mengklaim menawarkan bantuan keuangan sebesar 2 miliar dolar.

![Screenshot from 2024-05-25 16-59-59](https://github.com/S3N4T0R-0X0/Primitive-Bear-APT/assets/121706460/44745418-6d38-4bcc-bc42-368227fe63c0)

File Word ini akan digunakan untuk menempatkan payload skrip VBS setelah dilakukan obfuscation, yang di sini akan membantu membuat deteksi lebih sulit saat menempatkan skrip VBS ini ke dalam macro di file Word.


## Tahap Kedua (Payload VBScript)

Pertama saya akan membuat payload VBS, yang merupakan skrip VBS sederhana yang dirancang untuk membangun koneksi balik ke server C2, kemudian membuka file Word, mengaktifkan macro, dan menyisipkan payload ke dalam macro. Terakhir, saya akan menyimpan dokumen tersebut.

![Screenshot from 2024-05-25 18-54-11](https://github.com/S3N4T0R-0X0/Primitive-Bear-APT/assets/121706460/c6389cb6-2d22-44ec-9bac-bb3db6d153d6)

## Tahap Ketiga (Obfuscation Payload VBS)

Namun, sebelum saya menempatkan payload VBS ke dalam macro, saya akan melakukan obfuscation pada skrip tersebut agar lebih sulit dideteksi, dan saya menggunakan obfuscator VBS online untuk melakukan obfuscation: [VBS Obfuscator](https://isvbscriptdead.com/vbs-obfuscator/)

![Screenshot from 2024-05-25 18-43-46](https://github.com/S3N4T0R-0X0/Primitive-Bear-APT/assets/121706460/43336935-c058-4f96-9847-478951c6eccc)

## Tahap Keempat (Teknik Penanaman)

Sekarang saya akan menempatkan payload VBS yang telah di-obfuscate ke dalam file Microsoft Word dengan membuka menu View, mengklik Macros, dan membuat file macro baru.

![Screenshot from 2024-05-25 18-16-38](https://github.com/S3N4T0R-0X0/Primitive-Bear-APT/assets/121706460/34e0bade-fc71-4646-ba23-cad761920f96)

Simpan file Word dengan payload VBScript yang telah di-obfuscate yang tertanam di dalam macro, sehingga saya dapat mengeksekusi file payload ketika membuka file Word tersebut.

![Screenshot from 2024-05-25 20-04-19](https://github.com/S3N4T0R-0X0/Primitive-Bear-APT/assets/121706460/5dca0cd3-2ef8-483b-bd38-c1b902c0b5e6)


## Tahap Kelima (Membuat Arsip SFX)

Sekarang saya akan membuat Arsip SFX menggunakan WinRAR dan mengambil file SFX yang berisi Dokumen Word di dalamnya dengan payload VBS yang telah di-obfuscate melalui macro dan mengirimkannya dalam serangan spear phishing.

1. Buka WinRAR dan pilih file yang akan dimasukkan dalam arsip.

2. Pergi ke menu "Add" dan pilih "Add to archive..."

3. Di jendela "Archive name and parameters", pilih "SFX" sebagai format arsip.

4. Atur opsi SFX sesuai keinginan, termasuk jalur ekstraksi dan parameter eksekusi.

![Screenshot from 2024-05-26 09-23-43](https://github.com/S3N4T0R-0X0/Primitive-Bear-APT/assets/121706460/dafe9156-4b6f-4712-97de-cd2d4734439b)

## Hasil Akhir (Payload Terhubung ke Server C2)

Skrip C2 Perl ini memungkinkan untuk melakukan komunikasi jarak jauh dengan memanfaatkan enkripsi DES untuk transmisi data yang aman antara server penyerang dan target.

- **get_attacker_info dan get_port**: Meminta alamat IP dan nomor port.
- **get_des_key**: Meminta kunci DES sepanjang 8 byte.
- **encrypt_data**: Mengenkripsi hasil perintah menggunakan DES dengan padding.
- **main**: Menyiapkan server TCP, menerima koneksi, mengeksekusi perintah, mengenkripsi hasil, dan mengirimkannya ke klien.

[GitHub - SFX Archive dan Payload C2 Server](https://github.com/S3N4T0R-0X0/Primitive-Bear-APT/assets/121706460/e226ac70-42de-4f84-9e15-7f8ac2b47836)
