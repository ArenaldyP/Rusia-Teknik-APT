# Simulasi Serangan APT Energetic Bear
Ini adalah simulasi serangan oleh kelompok APT (Energetic Bear) yang menargetkan eWon, produsen peralatan SCADA dan jaringan industri asal Belgia.
Kampanye serangan ini aktif sejak Januari 2014. Rantai serangan dimulai dengan file XDP berbahaya yang berisi eksploitasi PDFSWF (CVE-2011-0611)
dan digunakan dalam serangan spear-phishing. Eksploitasi ini membuang DLL loader yang disimpan dalam bentuk terenkripsi dalam file XDP.
Eksploitasi dikirimkan sebagai file XDP (Paket Data XML) yang sebenarnya merupakan file PDF yang dikemas dalam wadah XML. Saya mengandalkan Kaspersky untuk mencari tahu detailnya guna membuat simulasi ini

https://media.kasperskycontenthub.comwp-contentuploadssites4320180308080817EB-YetiJuly2014-Public.pdf

![1](https://github.com/user-attachments/assets/efe8671a-8d0e-4d59-950f-f5e28d655522)


Serangan ini mencakup beberapa tahap, termasuk eksploitasi kerentanan (CVE-2011-0611) yang memungkinkan penyerang untuk menimpa pointer dalam memori dengan menanamkan .swf yang dibuat khusus. File XDP berisi eksploitasi SWF CVE-2011-0611 dan dua file yang dienkripsi dengan XOR disimpan dalam file XDP. Salah satu file adalah DLL berbahaya, yang lainnya adalah file JAR yang digunakan untuk menyalin dan menjalankan DLL dengan menjalankan baris perintah Cmd

1. CVE-2011-0611 modul ini mengeksploitasi kerentanan kerusakan memori dalam Adobe Flash Player versi 10.2.153.1 dan sebelumnya. Saya membuat versi Modifikasi dari eksploitasi tersebut berdasarkan Windows 10.

2. CVE-2012-1723 eksploitasi ini memungkinkan sandbox escape dan eksekusi kode jarak jauh pada target mana pun dengan JRE yang rentan (Java IE 8).

3. File XDP file XDP ini berisi XML Data Package (XDP) berbahaya dengan eksploitasi SWF (CVE-2011-0611). File ini juga mencakup fungsionalitas untuk mengunduh file tambahan melalui HTML-Smuggling oleh host apache.

4. Penyelundupan HTML file html-smuggling digunakan setelah diunggah ke server apache untuk mengunduh file lain. Salah satu file adalah muatan DLL, yang lainnya adalah file JAR kecil.

5. File JAR file jar ini digunakan untuk menyalin dan menjalankan DLL dengan menjalankan perintah cmd.

6. Muatan DLL penyerang menggunakan trojan havex, havex memindai sistem yang terinfeksi untuk menemukan kontrol pengawasan dan akuisisi data SCADA.

7. Dienkripsi dengan XOR file XDP berisi eksploitasi SWF dan dua file dienkripsi dengan XOR.

8. PHP backend C2-Server penyerang menggunakan situs web yang diretas sebagai backend PHP C2 Server sederhana.

9. Hasil akhir melakukan komunikasi jarak jauh dengan memanfaatkan enkripsi XOR untuk transmisi data yang aman antara server penyerang dan target.

![2](https://github.com/user-attachments/assets/b350c15b-21b8-4e8c-8e1f-188bd586c2e2)

## Tahap pertama (eksploitasi Kerentanan Korupsi Memori Adobe SWF CVE-2011-0611)

Modul ini mengeksploitasi kerentanan korupsi memori (CVE-2011-0611) di Adobe Flash Player
versi 10.2.153.1 dan versi sebelumnya. Kerentanan ini memungkinkan eksekusi kode arbitrer dengan
mengeksploitasi kelemahan dalam cara Adobe Flash Player menangani file .swf tertentu yang dibuat. Dengan memanfaatkan
kerentanan ini, penyerang dapat mengeksekusi kode arbitrer pada sistem korban.

![3](https://github.com/user-attachments/assets/5e4c50a5-50d7-434a-b295-05737514c927)


`sudo cp EnergeticBear_exploit.rb usrsharemetasploit-frameworkmodulesexploits`

`sudo updatedb`

`msf6  search EnergeticBear_exploit`

![4](https://github.com/user-attachments/assets/3b3ce655-6ae0-4174-8655-11737492fd47)


Versi Modifikasi dari exploit CVE-2011-0611 berbasis Windows 10, exploit asli dari https://github.comrapid7metasploit-frameworkblobmastermodulesexploitswindowsbrowseradobe_flashplayer_flash10o.rb

## Tahap Kedua (CVE-2012-1723 Oracle Java Applet Field Bytecode Verifier Cache RCE)

Kerentanan ini dalam komponen Java Runtime Environment (JRE) di Oracle Java SE 7 pembaruan 4 dan sebelumnya, 6 pembaruan 32 dan sebelumnya, 5 pembaruan 35 dan sebelumnya, dan 1.4.2_37 dan sebelumnya memungkinkan penyerang jarak jauh memengaruhi kerahasiaan, integritas, dan ketersediaan melalui vektor yang tidak diketahui terkait dengan Hotspot. jika Anda ingin tahu lebih banyak tentang CVE-2012-1723: https://www.microsoft.com/en-us/wdsi/threats/malware-encyclopedia-description?Name=Exploit:Java/CVE-2012-1723!generic&threatId=-2147302241

![5](https://github.com/user-attachments/assets/04290e3f-6268-480a-9649-d73c3fd30c1c)


`gunakan exploit/multi/browser/java_verifier_field_access`

Penyerang secara aktif membahayakan situs web yang sah untuk serangan watering hole. Situs web yang diretas ini kemudian mengarahkan korban ke file JAR atau HTML berbahaya yang dihosting di situs lain yang dikelola oleh kelompok tersebut (memanfaatkan CVE-2013-2465, CVE-2013-1347, dan CVE-2012-1723 di Java 6, Java 7, IE 7, dan IE 8). Situs web yang diretas ini akan menggunakan backend PHP C2 Server yang sederhana. 

![6](https://github.com/user-attachments/assets/848e8f5d-fa3f-45c5-84dc-c2ece3154069)



## Tahap ketiga (Paket Data XML XDP dengan eksploitasi SWF)

Eksploitasi dikirimkan sebagai file XDP (Paket Data XML) yang sebenarnya merupakan file PDF yang dikemas dalam kontainer XML. Ini adalah metode pengaburan PDF yang dikenal dan berfungsi sebagai lapisan antideteksi tambahan. jika Anda ingin tahu lebih banyak tentang berkas XDP: https://filext.com/file-extension/XDP

![7](https://github.com/user-attachments/assets/50fa5ad7-de31-43b4-b533-777c1a2ef62e)


Berkas XDP berisi eksploitasi SWF (CVE-2011-0611) dan dua berkas (dienkripsi dengan XOR) yang disimpan dalam berkas PDF. Berkas ini juga mencakup fungsionalitas untuk mengunduh berkas tambahan melalui HTML-Smuggling oleh host apache.

## Tahap keempat (Penyelundupan HTML dengan payload DLL & file JAR)

File penyelundupan HTML digunakan setelah diunggah ke server apache untuk mengunduh file lainnya. Salah satu file adalah payload DLL, yang lainnya adalah file JAR kecil yang digunakan untuk menyalin dan menjalankan DLL. Baris perintah untuk membuat payload base64 kemudian memasukkannya ke dalam file penyelundupan HTML: `base64 payload.dll -w 0` dan perintah yang sama tetapi dengan file jar.

![8](https://github.com/user-attachments/assets/0e738bc1-5ba0-46e1-9642-393fa269a84d)


## Tahap kelima (Salin DLL dengan file JAR)

File jar ini digunakan untuk menyalin dan menjalankan DLL dengan menjalankan perintah berikut:
`cmd /c copy payload.dll %TEMP%\\payload.dll /y & rundll32.exe %TEMP%\\payload.dll,RunDllEntry`

File ini membuat perintah untuk menyalin file bernama payload.dll ke direktori %TEMP% (biasanya direktori sementara) sebagai payload.dll lalu menjalankannya menggunakan rundll32.exe dan menunggu proses selesai menggunakan process.waitFor().

![9](https://github.com/user-attachments/assets/5254e357-37af-4161-b03e-6fccea4a7056)


## Muatan DLL tahap keenam (trojan Havex)

Penyerang memperoleh akses ke situs FTP eWon dan mengganti berkas yang sah dengan berkas yang terikat dengan dropper Havex beberapa kali.

Fungsionalitas utama komponen ini adalah mengunduh dan memuat modul DLL tambahan ke dalam
memori. Modul-modul ini disimpan di situs web yang disusupi yang bertindak sebagai server C&C. Untuk melakukannya,
malware menyuntikkan dirinya ke dalam proses EXPLORER.EXE, mengirimkan permintaan GET/POST ke skrip PHP
di situs web yang disusupi, lalu membaca dokumen HTML yang dikembalikan oleh skrip, mencari
data terenkripsi base64 di antara dua string “havex” di tag komentar `<!--havexhavex-->`
dan menulis data ini ke berkas %TEMP%\<tmp>.xmd (nama berkas dihasilkan oleh fungsi GetTempFilename).

Pengungkapan Lengkap tentang Trojan Havex: https://www.netresec.com/?page=Blog&month=2014-10&post=Full-Disclosure-of-Havex-Trojans

![10](https://github.com/user-attachments/assets/66ce4920-8d90-4a45-bf79-0bb1090b04c0)


Jika Anda ingin tahu lebih banyak tentang trojan Havex: https://malpedia.caad.fkie.fraunhofer.de/details/win.havex_rat

Catatan tentang trojan havex: http://pastebin.com/qCdMwtZ6

Dalam simulasi ini saya menggunakan payload sederhana dengan enkripsi XOR untuk mengamankan koneksi antara Server C2 dan Mesin Target, muatan ini menggunakan Winsock untuk membuat koneksi TCP antara mesin target dan mesin penyerang, dalam loop tak terbatas muatan menerima perintah dari penyerang, c2 mendekripsi perintah tersebut menggunakan enkripsi (XOR), mengeksekusi perintah tersebut menggunakan sistem, lalu tidur selama 10 detik sebelum mengulang loop.

![11](https://github.com/user-attachments/assets/c3c3ef4f-fc73-4629-acde-89f40727f1fa)


Formulir forensik jaringan (peretas SCADA) tentang trojan havex: https://scadahacker.com/library/Documents/Cyber_Events/NETRESEC%20-%20SCADA%20Network%20Forensics.pdf

## Tahap ketujuh (XDP terenkripsi dengan XOR)

Setelah melakukan kompilasi untuk payload dan file jar serta membuat base64 untuk file jar dan payload DLL, saya menaruhnya di file html smuggling, lalu saya membuat host untuk file html, lalu saya letakkan host ini di berkas XDP di samping CVE-2011-0611, lalu saya buat enkripsi XOR untuk berkas XDP, setelah ini ubah xdp ke pdf.

![12](https://github.com/user-attachments/assets/07ad46df-4469-44f7-abd6-54e2e6358a69)


saya menggunakan browserling untuk membuat enkripsi XOR: https://www.browserling.com/tools/xor-encrypt

## Tahap kedelapan (Server C2 backend PHP)

Skrip server PHP C2 ini memungkinkan untuk membuat komunikasi jarak jauh dengan memanfaatkan enkripsi XOR untuk transmisi data yang aman antara server penyerang dan target.

![13](https://github.com/user-attachments/assets/136e7e6d-1039-44a7-a6ef-02d701665fcb)


`xor_encrypt($data, $key)` Fungsi ini mengambil dua parameter: data yang akan dienkripsi ($data) dan kunci enkripsi ($key). Fungsi ini mengiterasi setiap karakter dalam data dan melakukan operasi XOR antara karakter dan karakter terkait dalam kunci (menggunakan modulo untuk mengulang kunci jika lebih pendek dari data), hasilnya dirangkai untuk membentuk output terenkripsi yang dikembalikan.

`send_to_payload($socket, $data, $encryption_key)` Fungsi ini mengirimkan data terenkripsi ke sistem target (payload) melalui koneksi soket. Pertama-tama, fungsi ini mengenkripsi data menggunakan fungsi xor_encrypt dengan kunci enkripsi yang diberikan, kemudian menulis data terenkripsi ke soket menggunakan socket_write.

`receive_from_payload($socket, $buffer_size, $encryption_key)` Fungsi ini menerima data terenkripsi dari sistem target melalui koneksi soket. Fungsi ini membaca data dari soket dengan ukuran buffer maksimum yang ditentukan oleh $buffer_size. Data terenkripsi yang diterima kemudian didekripsi menggunakan fungsi xor_encrypt dengan kunci enkripsi yang diberikan sebelum dikembalikan.

Jika Anda memilih (perintah atau URL), data dienkripsi menggunakan enkripsi XOR dengan kunci yang ditentukan pengguna sebelum dikirim ke target.

![14](https://github.com/user-attachments/assets/09998b7c-400f-451a-8da6-877c3b87af48)


Simulasi lain untuk serangan yang sama oleh cobaltstrike: https://www.youtube.com/watch?v=XkBvo6z0Tqo

## Hasil akhir: payload terhubung ke server PHP C2

1.Siapkan server web atau server HTTP apa pun yang dapat menyajikan konten teks.

2.Unggah file teks yang berisi perintah yang Anda inginkan agar dijalankan oleh sistem yang disusupi.

3.Pastikan file teks dapat diakses melalui HTTP dan catat URL-nya.

4.Saat diminta oleh skrip, masukkan URL yang Anda peroleh pada langkah tersebut.

CATATAN: Jika Anda memilih untuk mengambil perintah dari URL, Anda akan diminta untuk memasukkan URL. Jika Anda memilih untuk memasukkan perintah secara langsung, Anda akan diminta untuk memasukkan perintah yang akan dijalankan



https://github.com/user-attachments/assets/e9b41d14-c7b8-45f0-97a7-9af2a2649f0a


