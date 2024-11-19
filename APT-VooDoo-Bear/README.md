# Simulasi Serangan Voodoo Bear APT44

Ini adalah simulasi serangan oleh kelompok APT44 (Voodoo Bear) yang menargetkan entitas di Eropa Timur. Kampanye serangan ini aktif sejak pertengahan tahun 2022.  
Rantai serangan dimulai dengan backdoor berupa DLL yang menargetkan lingkungan Windows 32-bit dan 64-bit. Backdoor ini mengumpulkan informasi, membuat sidik jari pengguna dan mesin, lalu mengirimkan informasi tersebut ke server Command and Control (C2) yang dikendalikan penyerang. Backdoor ini menggunakan pendekatan multi-threaded dan memanfaatkan event object untuk sinkronisasi data dan sinyal antar thread.  
Saya mengacu pada laporan dari WithSecure untuk mendapatkan detail yang dibutuhkan dalam pembuatan simulasi ini: [Laporan WithSecure tentang Kapeka](https://labs.withsecure.com/publications/kapeka).

![1](https://github.com/user-attachments/assets/d2958d99-4ad5-4a6a-a2ec-8a9b425d431a)


Kapeka, yang berarti "bangau kecil" dalam bahasa Rusia, adalah backdoor fleksibel yang ditulis dalam C++. Backdoor ini memungkinkan aktor ancaman untuk menggunakannya sebagai toolkit tahap awal, sekaligus memberikan persistensi jangka panjang di jaringan korban.  
Dropper Kapeka adalah executable Windows 32-bit yang digunakan untuk menanam dan menjalankan backdoor di mesin korban. Dropper ini juga mengatur persistensi dengan membuat *scheduled task* atau entri registry autorun. Akhirnya, dropper menghapus dirinya sendiri dari sistem.  
Informasi lebih lanjut tentang backdoor Kapeka untuk kelompok APT Voodoo Bear dapat ditemukan di sini: [Kapeka Backdoor - PolySwarm Blog](https://blog.polyswarm.io/voodoo-bears-kapeka-backdoor-targets-critical-infrastructure).

---

## Tahap Pertama: RSA C2-Server

Script server C2 berbasis PHP ini memungkinkan komunikasi jarak jauh dengan menggunakan enkripsi RSA untuk transmisi data yang aman antara server penyerang dan target.

![2](https://github.com/user-attachments/assets/5a89b385-bf5a-4662-a4ec-df6c6d9ac755)


**`rsa_encrypt($data, $public_key):`**  
- **Tujuan**: Mengenkripsi data menggunakan kunci publik RSA.  
- **Proses**: Fungsi ini menerima data dan kunci publik sebagai input, lalu menggunakan `openssl_public_encrypt` untuk mengenkripsi data dengan kunci publik yang diberikan.  
- **Output**: Mengembalikan data yang telah dienkripsi.  

**`rsa_decrypt($data, $private_key):`**  
- **Tujuan**: Mendekripsi data menggunakan kunci privat RSA.  
- **Proses**: Fungsi ini menerima data terenkripsi dan kunci privat sebagai input, lalu menggunakan `openssl_private_decrypt` untuk mendekripsi data dengan kunci privat yang diberikan.  
- **Output**: Mengembalikan data yang telah didekripsi.  

![3](https://github.com/user-attachments/assets/eeced53c-9be7-4a73-8997-75bd56f90daf)


---

## Tahap Kedua: Pengujian Payload

Saya menggunakan payload sederhana yang ditulis dalam Python untuk menguji koneksi C2 (file: `testing_payload.py`). Tujuannya adalah memastikan tidak ada masalah dalam koneksi sebelum menulis payload sebenarnya.

![4](https://github.com/user-attachments/assets/41026421-a47d-44c9-bbf3-eb0d9c625247)


- **RSA dan PKCS1_OAEP dari pycryptodome**: Digunakan untuk enkripsi dan dekripsi dengan RSA.  
- **`rsa_encrypt(data, public_key):`** Mengenkripsi data menggunakan kunci publik yang diberikan.  
- **`rsa_decrypt(data, private_key):`** Mendekripsi data menggunakan kunci privat (tidak digunakan dalam script ini).  

![5](https://github.com/user-attachments/assets/6a4873cc-123a-472c-95a1-9767293f35b1)


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

![6](https://github.com/user-attachments/assets/709e5c1b-2d63-43f1-8173-3031c26861c6)


3. **Thread Ketiga**  
   - Memantau tugas yang masuk untuk diproses.  
   - Thread ini meluncurkan thread tambahan untuk menjalankan setiap tugas yang diterima.  

4. **Thread Keempat**  
   - Memantau penyelesaian tugas.  
   - Mengirim kembali hasil tugas yang telah diproses ke C2.  

![7](https://github.com/user-attachments/assets/843a71a9-c453-45e7-991e-1dae01ff5314)


### Kompilasi Manual:
Untuk mengkompilasi backdoor, gunakan perintah berikut:  
```bash
x86_64-w64-mingw32-g++ -shared -o kapeka_backdoor.dll kapeka_backdoor.cpp -lws2_32


rundll32.exe kapeka_backdoor.dll,ExportedFunction -d
