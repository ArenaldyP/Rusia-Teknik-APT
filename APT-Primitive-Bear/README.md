# Simulasi Serangan APT Primitive Bear

Ini adalah simulasi serangan oleh kelompok APT (Primitive Bear) yang menargetkan Layanan Migrasi Negara Ukraina. Kampanye serangan ini aktif dari bulan Desember hingga Juni 2021. Rantai serangan dimulai dengan dokumen Word yang dikirimkan ke korban melalui email, kemudian payload VBS digunakan untuk mendapatkan kendali dan komunikasi (Command and Control). Sebelum menempatkan payload atau menyuntikkannya ke dalam file Word, dilakukan obfuscation pada payload untuk menghindari deteksi, kemudian payload tersebut disuntikkan melalui macro ke dalam dokumen Word. Selanjutnya, saya membuat arsip SFX dan memasukkan file Word dengan payload di dalamnya untuk mendapatkan kendali dan komunikasi. Arsip SFX ini digunakan untuk melakukan serangan spear phishing, dan saya memperoleh kendali dengan membuka file Word. Saya mengandalkan Palo Alto Networks untuk mendapatkan detail serangan ini untuk membuat simulasi ini: [Palo Alto Networks Update](https://unit42.paloaltonetworks.com/gamaredon-primitive-bear-ukraine-update-2021/)

![1](https://github.com/user-attachments/assets/0ed24550-f7e9-476d-b94e-d90307dafc90)


Serangan ini melibatkan beberapa tahap, termasuk membuat file SFX dengan file Word di dalamnya. File Word ini berisi skrip VBS yang bertanggung jawab untuk command and control dan membuat obfuscation pada skrip VBS payload sebelum menempatkannya ke dalam file Word. Payload ini dikirim melalui serangan spear phishing dan melakukan komunikasi jarak jauh dengan memanfaatkan enkripsi DES untuk transmisi data yang aman antara server penyerang dan target.

1. **Membuat Dokumen Word**: Menulis dokumen Word (.doc atau .docx) yang berisi macro dengan payload VBS yang sudah di-obfuscate. Macro ini harus dirancang untuk mengeksekusi payload saat dokumen dibuka.

2. **Membuat Payload VBScript**: Membuat payload VBS yang dirancang untuk membangun koneksi balik (reverse connection) ke server Command and Control (C2).

3. **Melakukan Obfuscation pada Payload VBS**: Melakukan obfuscation pada payload VBS agar lebih sulit dideteksi oleh perangkat lunak antivirus atau solusi keamanan lainnya.

4. **Membuat Arsip Self-Extracting dengan WinRAR**: Menggunakan WinRAR untuk membuat arsip self-extracting (SFX).
Menambahkan dokumen Word yang berisi macro dan payload VBS yang di-obfuscate ke dalam arsip.

5. **Menempatkan Payload VBS yang di-obfuscate dan file Word di dalam arsip SFX untuk dikirim ke target**.

6. **Hasil Akhir**: Melakukan komunikasi jarak jauh dengan memanfaatkan enkripsi DES untuk transmisi data yang aman antara server penyerang dan target.

![2](https://github.com/user-attachments/assets/a9f30f52-0d5e-4e4a-9217-c06b80bd8764)


## Tahap pertama (teknik pengiriman)

Saya mulai dengan menyusun email phishing dalam dokumen Word untuk serangan yang akan datang. Setelah itu, sebelum membuat payload, yang akan terdiri dari skrip VBS yang disuntikkan ke dalam macro, saya akan mengenkapsulasinya dalam file SFX. Serangan ini menargetkan Departemen Imigrasi Ukraina, dengan surat phishing yang mengklaim menawarkan bantuan keuangan sebesar 2 miliar dolar.

![3](https://github.com/user-attachments/assets/1dbdb59b-bd66-4c93-80a9-cda7194062ab)


File Word ini akan digunakan untuk menempatkan payload skrip VBS setelah dilakukan obfuscation, yang di sini akan membantu membuat deteksi lebih sulit saat menempatkan skrip VBS ini ke dalam macro di file Word.


## Tahap Kedua (Payload VBScript)

Pertama saya akan membuat payload VBS, yang merupakan skrip VBS sederhana yang dirancang untuk membangun koneksi balik ke server C2, kemudian membuka file Word, mengaktifkan macro, dan menyisipkan payload ke dalam macro. Terakhir, saya akan menyimpan dokumen tersebut.

![4](https://github.com/user-attachments/assets/ae2f58ce-6cc9-44c6-9890-1629d3e5ccbf)


## Tahap Ketiga (Obfuscation Payload VBS)

Namun, sebelum saya menempatkan payload VBS ke dalam macro, saya akan melakukan obfuscation pada skrip tersebut agar lebih sulit dideteksi, dan saya menggunakan obfuscator VBS online untuk melakukan obfuscation: [VBS Obfuscator](https://isvbscriptdead.com/vbs-obfuscator/)

![5](https://github.com/user-attachments/assets/d8929055-1bb8-4120-aaa3-224943b2e78b)


## Tahap Keempat (Teknik Penanaman)

Sekarang saya akan menempatkan payload VBS yang telah di-obfuscate ke dalam file Microsoft Word dengan membuka menu View, mengklik Macros, dan membuat file macro baru.

![6](https://github.com/user-attachments/assets/7c461c75-5479-4fb2-9ea7-d4abd47eb00c)


Simpan file Word dengan payload VBScript yang telah di-obfuscate yang tertanam di dalam macro, sehingga saya dapat mengeksekusi file payload ketika membuka file Word tersebut.

![7](https://github.com/user-attachments/assets/688ac429-08b0-42ac-85e3-a5c5f0ba5853)



## Tahap Kelima (Membuat Arsip SFX)

Sekarang saya akan membuat Arsip SFX menggunakan WinRAR dan mengambil file SFX yang berisi Dokumen Word di dalamnya dengan payload VBS yang telah di-obfuscate melalui macro dan mengirimkannya dalam serangan spear phishing.

1. Buka WinRAR dan pilih file yang akan dimasukkan dalam arsip.

2. Pergi ke menu "Add" dan pilih "Add to archive..."

3. Di jendela "Archive name and parameters", pilih "SFX" sebagai format arsip.

4. Atur opsi SFX sesuai keinginan, termasuk jalur ekstraksi dan parameter eksekusi.

![8](https://github.com/user-attachments/assets/741f9a58-a5c5-4c9f-9754-fbd5b15c42dc)


## Hasil Akhir (Payload Terhubung ke Server C2)

Skrip C2 Perl ini memungkinkan untuk melakukan komunikasi jarak jauh dengan memanfaatkan enkripsi DES untuk transmisi data yang aman antara server penyerang dan target.

- **get_attacker_info dan get_port**: Meminta alamat IP dan nomor port.
- **get_des_key**: Meminta kunci DES sepanjang 8 byte.
- **encrypt_data**: Mengenkripsi hasil perintah menggunakan DES dengan padding.
- **main**: Menyiapkan server TCP, menerima koneksi, mengeksekusi perintah, mengenkripsi hasil, dan mengirimkannya ke klien.



https://github.com/user-attachments/assets/845df9f9-692d-43dd-87c9-5749273ed1e0


