#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <syslog.h>
#include <string.h>
#include <time.h>
#include <signal.h>

// Deklarasi path secara global supaya bisa diakses semua fungsi
char path_contract[] = "/home/faafaaeyzaaar_/sisop2/soal_2/contract.txt";
char path_log[] = "/home/faafaaeyzaaar_/sisop2/soal_2/work.log";
// Fungsi untuk mendapatkan timestamp
void get_timestamp(char *buffer) {
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    strftime(buffer, 50, "%Y-%m-%d %H:%M:%S", t);
}

// Fungsi untuk membuat atau mengembalikan file contract.txt
void create_or_restore_contract(int is_restore) {
    char ts[50];
    get_timestamp(ts);
    FILE *f = fopen(path_contract, "w"); // Pakai path lengkap
    if (f) {
        fprintf(f, "“A promise to keep going, even when unseen.”\n");
        if (is_restore) {
            fprintf(f, "restored at: %s\n", ts);
        } else {
            fprintf(f, "created at: %s\n", ts);
        }
        fclose(f);
    }
}

// Handler saat daemon dimatikan (Syarat No. 6)
void handle_sigterm(int sig) {
    FILE *log = fopen(path_log, "a"); // Pakai path lengkap
    if (log) {
        fprintf(log, "We really weren’t meant to be together\n");
        fclose(log);
    }
    exit(0);
}

int main() {
    pid_t pid, sid;

    // 1. Proses Fork untuk membuat Daemon
    pid = fork();
    if (pid < 0) exit(EXIT_FAILURE);
    if (pid > 0) exit(EXIT_SUCCESS);

    umask(0);
    sid = setsid();
    if (sid < 0) exit(EXIT_FAILURE);

    // Pindah ke root agar tidak mengunci file system, 
    // tapi kita aman karena pakai path lengkap (absolut)
    if ((chdir("/")) < 0) exit(EXIT_FAILURE);

    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    // Register signal handler untuk SIGTERM
    signal(SIGTERM, handle_sigterm);

    // Inisialisasi awal
    time_t last_log_time = 0;
    const char *statuses[] = {"[awake]", "[drifting]", "[numbness]"};
    srand(time(NULL));

    // Buat file contract.txt pertama kali saat program jalan (Syarat No. 3)
    create_or_restore_contract(0);

    while (1) {
        time_t now = time(NULL);

        // A. Tulis ke work.log setiap 5 detik (Syarat No. 2)
        if (now - last_log_time >= 5) {
            FILE *log = fopen(path_log, "a");
            if (log) {
                fprintf(log, "still working... %s\n", statuses[rand() % 3]);
                fclose(log);
            }
            last_log_time = now;
        }

        // B. Cek apakah file contract.txt dihapus (Syarat No. 4)
        if (access(path_contract, F_OK) == -1) {
            create_or_restore_contract(1); // Restore dalam 1-2 detik (karena sleep 1)
        } 
        else {
            // C. Cek apakah isi file diubah (Syarat No. 5)
            FILE *f_check = fopen(path_contract, "r");
            if (f_check) {
                char line[256];
                if (fgets(line, sizeof(line), f_check)) {
                    // Jika baris pertama tidak mengandung janji setia
                    if (strstr(line, "“A promise to keep going, even when unseen.”") == NULL) {
                        FILE *log = fopen(path_log, "a");
                        if (log) {
                            fprintf(log, "contract violated.\n");
                            fclose(log);
                        }
                        fclose(f_check);
                        create_or_restore_contract(1); // Restore isinya
                    } else {
                        fclose(f_check);
                    }
                } else {
                    fclose(f_check);
                }
            }
        }

        sleep(1); // Jeda pengecekan agar tidak membebani CPU
    }

    return 0;
}
