<details>
<summary>Soal 1</summary>

### Membuat File `kasir_muthu.c`
<p align="justify">
Alasan membuat file kasir_muthu.c adalah untuk melakukan Automasi Manajemen File berbasis Proses. Di dunia Sistem Operasi, kita tidak ingin melakukan tugas repetitif secara manual yang rentan human error. Dengan program ini, kita menerapkan konsep _Process Control_ (_Fork, Exec, Wait_) untuk menciptakan sebuah sistem yang "mandiri". Program ini memastikan bahwa setiap langkah - mulai dari pembuatan brankas hingga pengarsipan - berjalan secara sinkron, tepat waktu, dan memiliki sistem deteksi error yang kuat. Jadi, jika satu langkah gagal, sistem akan berhenti secara cerdas tanpa merusak data lainnya.
</p>
Setelah memahami tujuan dari tugas ini, berikut adalah implementasi kode yang digunakan :

### Bagian Header (Library)
```bash
#include <stdio.h>
```
<p align="justify">
Standard Input Output. Library ini wajib ada karena program lo butuh melakukan interaksi ke layar. Di dalam kodingan ini, dia bertanggung jawab atas fungsi printf(). Tanpa ini, lo nggak bisa ngasih tau user kalau prosesnya sukses ([INFO]) atau gagal ([ERROR]).
</p>

```bash
#include <stdlib.h>
```
<p align="justify">
Standard Library. Library ini menyediakan fungsi-fungsi umum. Di kodingan ini, fungsi yang paling krusial adalah exit(). Ini dipakai buat menghentikan program secara paksa saat terjadi error, supaya sistem nggak menjalankan perintah selanjutnya yang bakal makin berantakan.
</p>

```bash
#include <unistd.h>
```
<p align="justify">
Unix Standard. Ini adalah library paling penting di Sistem Operasi berbasis Linux. Dia yang memfasilitasi komunikasi antara program C lo dengan sistem kernel. Di sini lo dapet akses ke fork() (buat belah proses) dan execlp() (buat ganti identitas proses jadi perintah terminal).
</p>

```bash
#include <sys/wait.h>
```
<p align="justify">
System Wait. Library ini fokus pada manajemen status proses. Lo pake fungsi waitpid() buat nahan si "Bapak" biar nggak jalan duluan. Dia juga nyediain makro WIFEXITED dan WEXITSTATUS buat nanya ke si "Anak": "Woi, tadi lu kerjanya bener nggak? Laporannya mana?"
</p>

```bash
#include <sys/types.h>
```
<p align="justify">
System Types. Library ini mendefinisikan tipe data khusus sistem. Contohnya pid_t. Di Linux, ID proses itu nggak cuma angka biasa, tapi punya format khusus agar sistem operasi bisa mengelolanya dengan efisien di dalam tabel proses.
</p>

### Bagian Fungsi (Check_Status)
```bash
void check_status(pid_t pid, int status) {
    if (WIFEXITED(status)) {
        if (WEXITSTATUS(status) != 0) {
            printf("[ERROR] Aiyaa! Proses gagal, file atau folder tidak ditemukan.\n");
            exit(1);
        }
    }
}
```
<p align="justify">
Fungsi ini bertindak sebagai Validator. Setiap kali si "Anak" selesai menjalankan tugas (seperti mkdir atau cp), si "Bapak" nggak langsung percaya gitu aja.
</p>

  * ```WIFEXITED(status)``` mengecek apakah proses anak selesai secara normal (nggak mati mendadak karena crash).
  * ```WEXITSTATUS(status) != 0``` mengecek kode keluar. Di Linux, kalau perintah sukses, kodenya 0. Kalau kodenya bukan 0, berarti ada masalah (misal: folder gagal dibuat karena sudah ada, atau file yang mau dicopy nggak ketemu).
  * Jika terdeteksi gagal, program langsung dimatikan (exit(1)) supaya langkah selanjutnya (seperti men-zip file yang nggak ada) tidak dijalankan.

### Bagian Logika Utama

**A. Membuat Folder (mkdir)**
```bash
pid = fork();
if (pid == 0) {
    execlp("mkdir", "mkdir", "brankas_kedai", NULL);
    exit(1);
} else {
    waitpid(pid, &status, 0);
    check_status(pid, status);
}
```
<p align="justify">
Proses pertama adalah Inisialisasi Tempat. Program menggunakan fork() untuk membuat duplikat dirinya. Si "Anak" (pid == 0) bertugas memanggil execlp untuk menjalankan perintah mkdir. Perintah ini akan menciptakan folder brankas_kedai. Si "Bapak" (else) akan berhenti sejenak menggunakan waitpid() sampai folder tersebut benar-benar tercipta. Ini penting agar langkah penyalinan file nanti punya tujuan folder yang valid.
</p>

**B. Menyalin File & Filter Data**
```bash
execlp("cp", "cp", "buku_hutang.csv", "brankas_kedai/", NULL);
// ... dan ...
execlp("bash", "bash", "-c", "grep 'Belum Lunas' ... > ...", NULL);
```
<p align="justify">
Setelah folder ada, program membuat proses anak baru untuk menyalin buku_hutang.csv. Setelah disalin, program menjalankan filter menggunakan grep.
Kenapa pake bash -c? Karena tanda > (redirection) adalah fitur milik Shell (Bash), bukan fitur asli dari perintah grep. Kita butuh Bash untuk mengambil baris "Belum Lunas" dan memasukannya ke file baru bernama daftar_penunggak.txt. Ini adalah inti dari "tugas" Uncle Muthu untuk mendata penunggak hutang.
</p>

**C. Pengarsipan**
```bash
execlp("zip", "zip", "-r", "rahasia_muthu.zip", "brankas_kedai", NULL);
```
<p align="justify">
Langkah terakhir adalah Finalisasi. Semua file yang sudah dikumpulkan di dalam folder brankas_kedai dibungkus menjadi satu file kompresi bernama rahasia_muthu.zip. Parameter -r memastikan semua isi di dalam folder ikut terbawa. Ini adalah langkah pengamanan agar data hutang tersebut ringkas dan siap disimpan/dikirim.
</p>

### Hasil Akhir (Output)

Setelah semua langkah di atas selesai tanpa ada error di check_status, program akan mencetak baris terakhir:
```bash
printf("[INFO] Fuhh, selamat! Buku hutang dan daftar penagihan berhasil diamankan.\n");
```
Jika semua file (buku_hutang.csv) tersedia di folder tersebut, maka terminal akan menampilkan laporan kerja otomatis dari perintah Linux:

  1. ```(Hening - folder dibuat)```
  2. ```(Hening - file disalin & difilter)```
  3. ```adding: brankas_kedai/ (stored 0%)```
  4. ```adding: brankas_kedai/buku_hutang.csv (deflated 10%)```
  5. ```adding: brankas_kedai/daftar_penunggak.txt (deflated 5%)```
  6. ```[INFO] Fuhh, selamat! Buku hutang dan daftar penagihan berhasil diamankan.```
