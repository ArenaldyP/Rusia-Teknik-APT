# Simulasi Serangan Voodoo Bear APT44

Ini adalah simulasi serangan oleh kelompok APT44 (Voodoo Bear) yang menargetkan entitas di Eropa Timur. Kampanye serangan ini aktif sejak pertengahan tahun 2022.  
Rantai serangan dimulai dengan backdoor berupa DLL yang menargetkan lingkungan Windows 32-bit dan 64-bit. Backdoor ini mengumpulkan informasi, membuat sidik jari pengguna dan mesin, lalu mengirimkan informasi tersebut ke server Command and Control (C2) yang dikendalikan penyerang. Backdoor ini menggunakan pendekatan multi-threaded dan memanfaatkan event object untuk sinkronisasi data dan sinyal antar thread.  
Saya mengacu pada laporan dari WithSecure untuk mendapatkan detail yang dibutuhkan dalam pembuatan simulasi ini: [Laporan WithSecure tentang Kapeka](https://labs.withsecure.com/publications/kapeka).

![imageedit_2_8201736021](https://github.com/S3N4T0R-0X0/Voodoo-Bear-APT/assets/121706460/d8af69c6-b3f6-4870-a8d9-6dcf222c7564)

Kapeka, yang berarti "bangau kecil" dalam bahasa Rusia, adalah backdoor fleksibel yang ditulis dalam C++. Backdoor ini memungkinkan aktor ancaman untuk menggunakannya sebagai toolkit tahap awal, sekaligus memberikan persistensi jangka panjang di jaringan korban.  
Dropper Kapeka adalah executable Windows 32-bit yang digunakan untuk menanam dan menjalankan backdoor di mesin korban. Dropper ini juga mengatur persistensi dengan membuat *scheduled task* atau entri registry autorun. Akhirnya, dropper menghapus dirinya sendiri dari sistem.  
Informasi lebih lanjut tentang backdoor Kapeka untuk kelompok APT Voodoo Bear dapat ditemukan di sini: [Kapeka Backdoor - PolySwarm Blog](https://blog.polyswarm.io/voodoo-bears-kapeka-backdoor-targets-critical-infrastructure).

---

## Tahap Pertama: RSA C2-Server

Script server C2 berbasis PHP ini memungkinkan komunikasi jarak jauh dengan menggunakan enkripsi RSA untuk transmisi data yang aman antara server penyerang dan target.

![Screenshot dari 2024-06-13 22-11-36](https://github.com/S3N4T0R-0X0/Voodoo-Bear-APT/assets/121706460/acddd6d1-60cf-4ea9-a259-e844f2ac02a6)

**`rsa_encrypt($data, $public_key):`**  
- **Tujuan**: Mengenkripsi data menggunakan kunci publik RSA.  
- **Proses**: Fungsi ini menerima data dan kunci publik sebagai input, lalu menggunakan `openssl_public_encrypt` untuk mengenkripsi data dengan kunci publik yang diberikan.  
- **Output**: Mengembalikan data yang telah dienkripsi.  

**`rsa_decrypt($data, $private_key):`**  
- **Tujuan**: Mendekripsi data menggunakan kunci privat RSA.  
- **Proses**: Fungsi ini menerima data terenkripsi dan kunci privat sebagai input, lalu menggunakan `openssl_private_decrypt` untuk mendekripsi data dengan kunci privat yang diberikan.  
- **Output**: Mengembalikan data yang telah didekripsi.  

![Screenshot dari 2024-06-13 22-17-12](https://github.com/S3N4T0R-0X0/Voodoo-Bear-APT/assets/121706460/d0701451-ff05-4db8-95fe-e584d688f5b8)

---

## Tahap Kedua: Pengujian Payload

Saya menggunakan payload sederhana yang ditulis dalam Python untuk menguji koneksi C2 (file: `testing_payload.py`). Tujuannya adalah memastikan tidak ada masalah dalam koneksi sebelum menulis payload sebenarnya.

![Screenshot dari 2024-06-14 17-52-30](https://github.com/S3N4T0R-0X0/Voodoo-Bear-APT/assets/121706460/a87fcabf-f491-4b78-b5b3-300d779cafdd)

- **RSA dan PKCS1_OAEP dari pycryptodome**: Digunakan untuk enkripsi dan dekripsi dengan RSA.  
- **`rsa_encrypt(data, public_key):`** Mengenkripsi data menggunakan kunci publik yang diberikan.  
- **`rsa_decrypt(data, private_key):`** Mendekripsi data menggunakan kunci privat (tidak digunakan dalam script ini).  

![Screenshot dari 2024-06-14 17-45-29](https://github.com/S3N4T0R-0X0/Voodoo-Bear-APT/assets/121706460/e947a9df-4a03-4e3b-b4a6-d186ee2c0e04)

**Catatan**: Pastikan server mengirimkan perintah terenkripsi RSA dan menangani respons dengan benar. Script ini membutuhkan pustaka `pycryptodome` untuk enkripsi dan dekripsi RSA:  

    pip install pycryptodome

---

## Tahap Ketiga: Kapeka Backdoor

Backdoor Kapeka adalah file DLL Windows yang memiliki satu fungsi yang diekspor melalui ordinal2 (bukan melalui nama). Backdoor ini ditulis dalam C++ dan dikompilasi menggunakan Visual Studio 2017 (linker 14.16).  
File backdoor ini menyamar sebagai Add-In Microsoft Word dengan ekstensi `.wll`, tetapi sebenarnya merupakan file DLL.  

Saya telah mengembangkan simulasi backdoor Kapeka yang digunakan oleh penyerang dalam serangan nyata.

### Thread Utama dalam Backdoor:

1. **Thread Pertama**  
   - Merupakan thread utama yang melakukan inisialisasi dan rutinitas keluar.
   - Melakukan polling ke C2 untuk menerima tugas atau konfigurasi C2 terbaru.  

2. **Thread Kedua**  
   - Memantau event log off pada Windows.  
   - Memberikan sinyal ke thread utama untuk melakukan rutinitas keluar dengan baik saat pengguna log off.  

![Screenshot dari 2024-06-13 17-19-15](https://github.com/S3N4T0R-0X0/Voodoo-Bear-APT/assets/121706460/aef17efa-64ab-4cd4-8e8c-ff21ec9b6ce5)

3. **Thread Ketiga**  
   - Memantau tugas yang masuk untuk diproses.  
   - Thread ini meluncurkan thread tambahan untuk menjalankan setiap tugas yang diterima.  

4. **Thread Keempat**  
   - Memantau penyelesaian tugas.  
   - Mengirim kembali hasil tugas yang telah diproses ke C2.  

![Screenshot dari 2024-06-13 17-21-20](https://github.com/S3N4T0R-0X0/Voodoo-Bear-APT/assets/121706460/fa745546-7488-4f8b-a118-8c4866418b0c)

### Kompilasi Manual:
Untuk mengkompilasi backdoor, gunakan perintah berikut:  
```bash
x86_64-w64-mingw32-g++ -shared -o kapeka_backdoor.dll kapeka_backdoor.cpp -lws2_32


rundll32.exe kapeka_backdoor.dll,ExportedFunction -d
