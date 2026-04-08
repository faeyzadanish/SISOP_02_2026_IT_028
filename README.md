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
</details>

<details>
<summary>Soal 2</summary>

### Membuat File ```contract_daemon.c```
<p align="justify">
Tujuannya adalah untuk memahami mekanisme Background Process Monitoring dan Signal Handling. Program ini bertindak sebagai penjaga abadi bagi file <code>contract.txt</code>. Ia tidak hanya memastikan file tersebut selalu ada, tapi juga memastikan isinya tidak dikhianati (diubah). Jika terjadi modifikasi atau penghapusan, Daemon akan langsung memulihkannya dalam hitungan detik. Ini adalah implementasi nyata dari sistem auto-healing pada layanan server Linux.
</p>
Setelah memahami tujuan dari tugas ini, berikut adalah implementasi kode yang digunakan :

### Bagian Header (Library)
```bash
#include <sys/types.h>
```
Mendefinisikan tipe data sistem seperti ```pid_t``` agar identifikasi proses anak dan induk berjalan akurat.

```bash
#include <sys/stat.h>
```
Digunakan untuk mengatur ```umask(0)```, memastikan Daemon memiliki izin penuh untuk membuat atau merestore file.

```bash
#include <time.h>
```
Sangat Penting. Digunakan untuk mengambil waktu sistem agar setiap kejadian di ```contract.txt``` dan ```work.log``` memiliki stempel waktu yang valid.

```bash
#include <signal.h>
```
Krusial. Digunakan untuk menangkap sinyal ```SIGTERM``` (sinyal mematikan proses), sehingga Daemon bisa melakukan aksi terakhir sebelum berhenti.

```bash
#include <unistd.h>
```
Menyediakan fungsi ```fork()```, ```setsid()```, dan ```chdir()```, yang merupakan syarat mutlak pembentukan sebuah Daemon.

### Fungsi Pendukung
```bash
void handle_sigterm(int sig) {
    FILE *log = fopen("work.log", "a");
    if (log) {
        fprintf(log, "We really weren’t meant to be together\n");
        fclose(log);
    }
    exit(0);
}
```

* ```get_timestamp```: Fungsi ini memformat waktu menjadi string yang manusiawi (YYYY-MM-DD HH:MM:SS) untuk pencatatan log.
* ```handle_sigterm```: Sesuai permintaan soal nomor 6, fungsi ini adalah Last Wish dari Daemon. Ketika kita menjalankan perintah kill pada PID Daemon ini, ia akan menulis pesan perpisahan melankolis ke work.log sebelum benar-benar mematikan dirinya sendiri.

### Pembentukan Daemon

Di dalam ```main()```, terdapat langkah-langkah isolasi proses:

* ```fork() & exit()```: Memastikan proses berjalan di background dan melepaskan kendali dari terminal.
* ```setsid()```: Membuat sesi baru agar Daemon tidak menjadi "Yatim Piatu" (Orphan) yang terikat pada sesi terminal lama.
* ```chdir("/")```: Memastikan Daemon bekerja dari root, agar tidak mengunci direktori kerja saat folder tersebut ingin di-unmount.
* ```close(STDIN/OUT/ERR)```: Menghilangkan keterkaitan dengan input/output terminal karena Daemon bekerja dalam keheningan.

### Infinite Loop

Ini adalah bagian utama di mana Daemon melakukan tugas pengawasannya setiap detik.

* **Interval Log (5 Detik)**: Menggunakan variabel last_log_time, Daemon secara rutin menulis status emosionalnya ([awake], [drifting], [numbness]) ke work.log.
* **Restorasi File (Syarat No 4)**: Menggunakan access (path_contract, F_OK), jika file contract.txt hilang, Daemon akan langsung memanggil fungsi restore dan mengganti namanya kembali ke path yang benar.
* **Deteksi Pelanggaran (Syarat No 5)**: Daemon membaca isi file menggunakan fgets dan mencocokkannya dengan kalimat asli. Jika kalimat "A promise to keep going..." hilang atau diubah, Daemon akan mencatat contract violated. dan menimpa file tersebut kembali ke versi aslinya.

### Output
```bash
still working... [awake]
still working... [drifting]
contract violated.              <-- Terdeteksi isi file dirubah
still working... [numbness]
We really weren’t meant to be together  <-- Saat Daemon di-kill
```
<p align="justify">
Hasil ini menunjukkan bahwa Daemon berhasil menjalankan tugasnya secara sinkron. Penggunaan <code>sleep(1)</code> memastikan pengecekan dilakukan sangat intensif (responsif), sehingga celah waktu bagi file untuk hilang atau rusak sangat kecil. Kalimat perpisahan di akhir log membuktikan bahwa sistem Signal Handling berfungsi sempurna dalam menangkap interupsi dari pengguna.
</p>
</details>

<details>
<summary>Soal 3</summary>

### Membuat File ```angel.c```

Tujuan utamanya adalah mempelajari Process Obfuscation (Penyamaran Proses) dan Data Security dasar. Program ini tidak hanya berjalan di background sebagai Daemon, tapi juga memiliki kemampuan untuk merubah namanya sendiri di daftar proses sistem agar tidak mudah dikenali. Selain itu, pesan yang dikirimkan dienkripsi menggunakan metode Base64, memastikan hanya mereka yang memiliki alat dekripsi yang bisa membaca isi pesannya. Ini merepresentasikan bagaimana layanan sistem operasi menyembunyikan identitasnya demi keamanan data.
</p>
Setelah memahami tujuan dari tugas ini, berikut adalah implementasi kode yang digunakan :

### Bagian Header
```bash
#include <sys/prctl.h>
```
Krusial. Library ini digunakan untuk mengakses fungsi ```prctl()```. Inilah "alat kosmetik" yang memungkinkan program merubah nama prosesnya menjadi "maya" saat sedang berjalan.

```bash
#include <stdint.h>
```
Menyediakan tipe data integer dengan ukuran spesifik (seperti ```uint32_t```). Ini sangat penting dalam algoritma enkripsi Base64 agar manipulasi bit data berjalan akurat.

```bash
#include <string.h>
```
Digunakan untuk mengolah pesan teks (quotes) dan membandingkan argumen input (-daemon, -kill, -decrypt) menggunakan strcmp.

```bash
#include <time.h>
```
Digunakan untuk mencatat waktu aktivitas pada ethereal.log sesuai format yang diminta oleh soal.

```bash
#include <unistd.h> & <stdlib.h>
```
Standar manajemen proses untuk melakukan fork(), setsid(), dan pengaturan alokasi memori pada fungsi enkripsi.

### Fungsi ```write log```
```bash
void write_log(const char *process, const char *status) {
    FILE *log = fopen("ethereal.log", "a");
    fprintf(log, "[%02d:%02d:%04d]-[%02d:%02d:%02d]_%s_%s\n", ...);
    fclose(log);
}
```
<p align="justify">
Fungsi ini adalah "buku harian" dari program. Setiap kali Daemon melakukan sesuatu (mengirim pesan, didekripsi, atau dimatikan), dia akan mencatatnya ke <code>ethereal.log</code>. Formatnya sangat spesifik sesuai permintaan soal: <code>[tanggal]-[waktu]_nama-proses_STATUS</code>. Ini berguna untuk memantau apakah Daemon masih hidup atau sudah dimatikan oleh user.
</p>

### Fungsi ```base 64```
```bash
char *base64_encode(const char *input) {
    static char table[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
}
```
<p align="justify">
Ini adalah jantung dari fitur "Surprise". Agar pesan cinta tidak bisa dibaca langsung oleh siapa pun yang membuka file <code>LoveLetter.txt</code>, teks asli diubah menjadi format <code>Base64</code>.

**Logikanya**: Dia mengambil 3 karakter teks asli (24 bit), lalu membaginya menjadi 4 bagian masing-masing 6 bit. 6 bit ini kemudian dipetakan ke karakter di dalam table (A-Z, a-z, 0-9). Hasilnya adalah string acak yang terlihat rapi namun rahasia.
</p>

### Fungsi ```run_daemon```
```bash
void run_daemon() {
    pid_t pid = fork();

    prctl(PR_SET_NAME, "maya", 0, 0, 0);
    write_log("maya", "RUNNING");

    while (1) {
        FILE *f = fopen("LoveLetter.txt", "w");
        sleep(10);
    }
}
```
<p align="justify">
    
* <code>prctl(PR_SET_NAME, "maya", ...)</code>: Inilah trik sulapnya. Meskipun nama file kodingan lo adalah angel.c, saat fungsi ini dipanggil, sistem akan melabeli proses ini sebagai "maya". Jadi kalau user mengetik ps aux, mereka tidak akan melihat angel, melainkan maya.
* **Looping 10 Detik**: Sesuai permintaan nomor 3 dan 4, Daemon ini akan terus berjalan selamanya. Setiap 10 detik, dia memilih satu kutipan galau secara acak, mengenkripsinya ke Base64, dan menuliskannya ke <code>LoveLetter.txt</code>.
</p>

### Fungsi ```main```

Bagian ini adalah gerbang utama yang menentukan apa yang akan dilakukan program berdasarkan argumen yang lo ketik di terminal.

* <code>-daemon</code>: Memanggil run_daemon(). Program akan "melepaskan diri" dari terminal dan mulai bekerja diam-diam sebagai "maya".
* <code>-kill</code>: Menjalankan perintah system("pkill -f maya"). Ini cara praktis untuk menghentikan Daemon. Program mencari proses bernama "maya" dan menghentikannya, lalu mencatat kesuksesan tersebut ke log.
* <code>-decrypt</code>:
  ```bash
  system("base64 -d LoveLetter.txt && echo ''");
  ```

  Ini adalah cara untuk membaca pesan rahasia. Program memanggil utilitas sistem ```base64``` untuk menerjemahkan kembali isi ```LoveLetter.txt``` ke bahasa manusia sehingga lo bisa membacanya di terminal.

### Output

**Command untuk run codenya :**
```bash
./angel -daemon
# 10 detik kemudian...
./angel -decrypt
```

**Output di Terminal :**
```bash
Isi surat (Decrypted):
aku mencintaimu dari sekarang hingga selamanya
```

**Log akhir ```ethereal.log``` :**
```bash
[08:04:2026]-[20:30:01]_maya_RUNNING
[08:04:2026]-[20:30:11]_secret_SUCCESS
[08:04:2026]-[20:30:11]_surprise_SUCCESS
[08:04:2026]-[20:30:45]_decrypt_SUCCESS
```

Codingan ini adalah implementasi lengkap dari **Daemon Process, Process Renaming, dan Data Security**. Dengan memisahkan logika menjadi fungsi-fungsi kecil, program ini menjadi sangat modular (mudah diubah-ubah) dan aman karena setiap aksi dicatat dengan rapi di dalam file log.
</details>
