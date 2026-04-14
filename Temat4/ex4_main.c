#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <time.h>
#include "pipe_utils.h"

static void producent(int pipe_write_fd, const char *src_path)
{
    int src_fd;
    ssize_t n;
    char buf[PRODUCER_CHUNK];
 
    src_fd = open(src_path, O_RDONLY);
    if (src_fd < 0)
        die("Producent: open (plik zrodlowy)");
 
    printf("[Producent] Uruchomiony. Czytam z '%s', porcje po %d B.\n",
           src_path, PRODUCER_CHUNK);
 
    while ((n = read(src_fd, buf, sizeof(buf))) > 0) {
        write(STDOUT_FILENO, "[Producent] Wysylam: ", 21);
        write(STDOUT_FILENO, buf, n);
        write(STDOUT_FILENO, "\n", 1);
 
        if (write(pipe_write_fd, buf, n) != n)
            die("Producent: write (potok)");
 
        sleep(rand() % (MAX_SLEEP + 1));
    }
 
    if (n < 0)
        die("Producent: read (plik zrodlowy)");
 
    close(src_fd);
    close(pipe_write_fd);
    printf("[Producent] Zakonczono. Wszystkie dane wyslane.\n");
}
 
static void konsument(int pipe_read_fd, const char *dst_path)
{
    int dst_fd;
    ssize_t n;
    char buf[CONSUMER_CHUNK];
 
    dst_fd = open(dst_path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (dst_fd < 0)
        die("Konsument: open (plik docelowy)");
 
    printf("[Konsument] Uruchomiony. Zapisuje do '%s', porcje po %d B.\n",
           dst_path, CONSUMER_CHUNK);
 
    while ((n = read(pipe_read_fd, buf, sizeof(buf))) > 0) {
        write(STDOUT_FILENO, "[Konsument] Odebrano: ", 22);
        write(STDOUT_FILENO, buf, n);
        write(STDOUT_FILENO, "\n", 1);

        if (write(dst_fd, buf, n) != n)
            die("Konsument: write (plik docelowy)");

        sleep(rand() % (MAX_SLEEP + 1));
    }
 
    if (n < 0)
        die("Konsument: read (potok)");
 
    close(dst_fd);
    close(pipe_read_fd);
    printf("[Konsument] Zakonczono. Wszystkie dane zapisane.\n");
}
 
int main(int argc, char *argv[])
{
    int pipefd[2];
    pid_t pid;
 
    if (argc != 3) {
        fprintf(stderr, "Uzycie: %s plik_zrodlowy plik_docelowy\n", argv[0]);
        exit(1);
    }
 
    srand((unsigned)time(NULL));
 
    if (pipe(pipefd) < 0)
        die("pipe");
 
    printf("=== Cwiczenie 4: Producent-Konsument (potok nienazwany) ===\n");
    printf("Zrodlo: %s  ->  Cel: %s\n\n", argv[1], argv[2]);
 
    pid = fork();
    if (pid < 0)
        die("fork");
 
    if (pid == 0) {
        close(pipefd[0]);
        producent(pipefd[1], argv[1]);
        exit(0);
    } else {
        close(pipefd[1]);
        konsument(pipefd[0], argv[2]);
        wait(NULL);
    }
 
    printf("\nOba procesy zakonczone. Sprawdz identycznosc plikow:\n");
    printf("  diff -s %s %s\n", argv[1], argv[2]);
 
    return 0;
}