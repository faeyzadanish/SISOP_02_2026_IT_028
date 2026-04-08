#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <time.h>
#include <signal.h>
#include <sys/prctl.h>
#include <stdint.h>

void write_log(const char *process, const char *status) {
    FILE *log = fopen("ethereal.log", "a");
    if (!log) return;
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    fprintf(log, "[%02d:%02d:%04d]-[%02d:%02d:%02d]_%s_%s\n",
            t->tm_mday, t->tm_mon + 1, t->tm_year + 1900,
            t->tm_hour, t->tm_min, t->tm_sec, process, status);
    fclose(log);
}

char *base64_encode(const char *input) {
    static char table[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    int len = strlen(input);
    char *out = malloc(4 * ((len + 2) / 3) + 1);
    int i, j;
    for (i = 0, j = 0; i < len; ) {
        uint32_t a = (unsigned char)input[i++];
        uint32_t b = i < len ? (unsigned char)input[i++] : 0;
        uint32_t c = i < len ? (unsigned char)input[i++] : 0;
        uint32_t combined = (a << 16) | (b << 8) | c;
        out[j++] = table[(combined >> 18) & 0x3F];
        out[j++] = table[(combined >> 12) & 0x3F];
        out[j++] = i > len + 1 ? '=' : table[(combined >> 6) & 0x3F];
        out[j++] = i > len ? '=' : table[combined & 0x3F];
    }
    out[j] = '\0';
    return out;
}

void run_daemon() {
    pid_t pid = fork();
    if (pid < 0) exit(EXIT_FAILURE);
    if (pid > 0) exit(EXIT_SUCCESS);
    if (setsid() < 0) exit(EXIT_FAILURE);

    prctl(PR_SET_NAME, "maya", 0, 0, 0);
    write_log("maya", "RUNNING");

    char *quotes[] = {
        "aku akan fokus pada diriku sendiri",
        "aku mencintaimu dari sekarang hingga selamanya",
        "aku akan menjauh darimu, hingga takdir mempertemukan kita di versi kita yang terbaik.",
        "kalau aku dilahirkan kembali, aku tetap akan terus menyayangimu"
    };

    while (1) {
        FILE *f = fopen("LoveLetter.txt", "w");
        if (f) {
            char *msg = quotes[rand() % 4];
            char *enc = base64_encode(msg);
            fprintf(f, "%s", enc);
            fclose(f);
            free(enc);
            write_log("secret", "SUCCESS");
            write_log("surprise", "SUCCESS");
        }
        sleep(10);
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) return 0;

    if (strcmp(argv[1], "-daemon") == 0) {
        run_daemon();
    } 
    else if (strcmp(argv[1], "-kill") == 0) {
        system("pkill -f maya");
        write_log("kill", "SUCCESS");
    }
    else if (strcmp(argv[1], "-decrypt") == 0) {
        if (access("LoveLetter.txt", F_OK) == 0) {
            printf("Isi surat (Decrypted):\n");
            system("base64 -d LoveLetter.txt && echo ''");
            write_log("decrypt", "SUCCESS");
        } else {
            write_log("decrypt", "ERROR");
        }
    }
    return 0;
}
