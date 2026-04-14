#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <time.h>
#include "pipe_utils.h"

#define FIFO_PATH "/tmp/ex5a_fifo"
 
static void usun_fifo(void)
{
    if (unlink(FIFO_PATH) < 0)
        perror("atexit: unlink (FIFO)");
    else
        printf("[atexit] Potok FIFO '%s' usuniety.\n", FIFO_PATH);
}

static void producent(const char *src_path)
{
    int fifo_fd, src_fd;
    ssize_t n;
    char buf[PRODUCER_CHUNK];
 
    src_fd = open(src_path, O_RDONLY);
    if (src_fd < 0)
        die("Producent: open (plik zrodlowy)");
 
    fifo_fd = open(FIFO_PATH, O_WRONLY);
    if (fifo_fd < 0)
        die("Producent: open (FIFO)");
 
    printf("[Producent] Uruchomiony. Czytam z '%s', porcje po %d B.\n",
           src_path, PRODUCER_CHUNK);
 
    while ((n = read(src_fd, buf, sizeof(buf))) > 0) {
        write(STDOUT_FILENO, "[Producent] Wysylam: ", 21);
        write(STDOUT_FILENO, buf, n);
        write(STDOUT_FILENO, "\n", 1);
 
        if (write(fifo_fd, buf, n) != n)
            die("Producent: write (FIFO)");
 
        sleep(rand() % (MAX_SLEEP + 1));
    }
 
    if (n < 0)
        die("Producent: read (plik zrodlowy)");
 
    close(src_fd);
    close(fifo_fd);
    printf("[Producent] Zakonczono.\n");
}

static void konsument(const char *dst_path)
{
    int fifo_fd, dst_fd;
    ssize_t n;
    char buf[CONSUMER_CHUNK];
 
    dst_fd = open(dst_path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (dst_fd < 0)
        die("Konsument: open (plik docelowy)");
 
    fifo_fd = open(FIFO_PATH, O_RDONLY);
    if (fifo_fd < 0)
        die("Konsument: open (FIFO)");
 
    printf("[Konsument] Uruchomiony. Zapisuje do '%s', porcje po %d B.\n",
           dst_path, CONSUMER_CHUNK);
 
    while ((n = read(fifo_fd, buf, sizeof(buf))) > 0) {
        write(STDOUT_FILENO, "[Konsument] Odebrano: ", 22);
        write(STDOUT_FILENO, buf, n);
        write(STDOUT_FILENO, "\n", 1);
 
        if (write(dst_fd, buf, n) != n)
            die("Konsument: write (plik docelowy)");
 
        sleep(rand() % (MAX_SLEEP + 1));
    }
 
    if (n < 0)
        die("Konsument: read (FIFO)");
 
    close(fifo_fd);
    close(dst_fd);
    printf("[Konsument] Zakonczono.\n");
}

int main(int argc, char *argv[])
{
    pid_t pid;
 
    if (argc != 3) {
        fprintf(stderr, "Uzycie: %s plik_zrodlowy plik_docelowy\n", argv[0]);
        exit(1);
    }
 
    srand((unsigned)time(NULL));
 
    printf("=== Cwiczenie 5(a): Producent-Konsument (FIFO, jeden program) ===\n");
    printf("Zrodlo: %s  ->  Cel: %s\n", argv[1], argv[2]);
    printf("FIFO: %s\n\n", FIFO_PATH);

    if (mkfifo(FIFO_PATH, 0644) < 0)
        die("mkfifo");

    atexit(usun_fifo);
 
    pid = fork();
    if (pid < 0)
        die("fork");
 
    if (pid == 0) {
        producent(argv[1]);
        exit(0);
    } else {
        konsument(argv[2]);
        wait(NULL);
    }
 
    printf("\nOba procesy zakonczone. Sprawdz identycznosc plikow:\n");
    printf("  diff -s %s %s\n", argv[1], argv[2]);
 
    return 0;
}