#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

void check_status(pid_t pid, int status) {
    if (WIFEXITED(status)) {
        if (WEXITSTATUS(status) != 0) {
            printf("[ERROR] Aiyaa! Proses gagal, file atau folder tidak ditemukan.\n");
            exit(1);
        }
    }
}

int main() {
    pid_t pid;
    int status;

    pid = fork();
    if (pid == 0) {
        execlp("mkdir", "mkdir", "brankas_kedai", NULL);
        exit(1);
    } else {
        waitpid(pid, &status, 0);
        check_status(pid, status);
    }

    pid = fork();
    if (pid == 0) {
        execlp("cp", "cp", "buku_hutang.csv", "brankas_kedai/", NULL);
        exit(1);
    } else {
        waitpid(pid, &status, 0);
        check_status(pid, status);
    }

    pid = fork();
    if (pid == 0) {
        execlp("bash", "bash", "-c", "grep 'Belum Lunas' brankas_kedai/buku_hutang.csv > brankas_kedai/daftar_penunggak.txt", NULL);
        exit(1);
    } else {
        waitpid(pid, &status, 0);
        check_status(pid, status);
    }

    pid = fork();
    if (pid == 0) {
        execlp("zip", "zip", "-r", "rahasia_muthu.zip", "brankas_kedai", NULL);
        exit(1);
    } else {
        waitpid(pid, &status, 0);
        check_status(pid, status);
    }

    printf("[INFO] Fuhh, selamat! Buku hutang dan daftar penagihan berhasil diamankan.\n");

    return 0;
}
