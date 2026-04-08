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
