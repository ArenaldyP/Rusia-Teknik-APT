@echo off
:: Membuat service baru dengan nama W64Time, menggunakan svchost.exe sebagai proses host.
:: Jenis service adalah shared (type= share) dan service akan otomatis dimulai saat boot (start= auto).
sc create W64Time binPath= "c:\windows\system32\svchost.exe -k TimeService" type= share start= auto

:: Mengatur nama tampilan (display name) dari service menjadi "Windows 64 Time".
sc config W64Time DisplayName= "Windows 64 Time"

:: Menambahkan deskripsi untuk service yang menjelaskan fungsinya.
sc description W64Time "Maintains date and time synchronization on all clients and servers in the network. If this service is stopped, date and time synchronization will be unavailable. If this service is disabled, any services that explicitly depend on it will fail to start."

:: Mendaftarkan service W64Time di bawah grup svchost dengan nama grup "TimeService".
reg add "HKLM\SOFTWARE\Microsoft\Windows NT\CurrentVersion\svchost" /v TimeService /t REG_MULTI_SZ /d "W64Time" /f

:: Menambahkan parameter untuk service di registry agar dapat berfungsi dengan konfigurasi tertentu.
:: Parameter pertama adalah ServiceDll, yang menunjuk ke file DLL yang akan digunakan oleh service.
reg add "HKLM\SYSTEM\CurrentControlSet\services\W64Time\Parameters" /v ServiceDll /t REG_EXPAND_SZ /d "%SystemRoot%\system32\w64time.dll" /f

:: Menambahkan parameter Hosts, digunakan untuk menyimpan informasi host tertentu (placeholder "REMOVED 5050").
reg add "HKLM\SYSTEM\CurrentControlSet\services\W64Time\Parameters" /v Hosts /t REG_SZ /d "REMOVED 5050" /f

:: Menambahkan parameter Security, yang menyimpan informasi keamanan (placeholder "<REMOVED>").
reg add "HKLM\SYSTEM\CurrentControlSet\services\W64Time\Parameters" /v Security /t REG_SZ /d "<REMOVED>" /f

:: Menambahkan parameter TimeLong, yang berfungsi sebagai nilai waktu tunggu panjang (300000 ms atau 5 menit).
reg add "HKLM\SYSTEM\CurrentControlSet\services\W64Time\Parameters" /v TimeLong /t REG_DWORD /d 300000 /f

:: Menambahkan parameter TimeShort, yang berfungsi sebagai nilai waktu tunggu pendek (5000 ms atau 5 detik).
reg add "HKLM\SYSTEM\CurrentControlSet\services\W64Time\Parameters" /v TimeShort /t REG_DWORD /d 5000 /f

:: Memulai service W64Time setelah konfigurasi selesai.
sc start W64Time

:: Menampilkan pesan bahwa setup service telah selesai dan menunggu input pengguna untuk melanjutkan.
echo Service setup completed.
pause
