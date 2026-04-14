#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include "pipe_utils.h"
 
int main(int argc, char *argv[])
{
    int fifo_fd, src_fd;
    ssize_t n;
    char buf[PRODUCER_CHUNK];
 
    if (argc != 3) {
        fprintf(stderr, "Uzycie: %s sciezka_fifo plik_zrodlowy\n", argv[0]);
        exit(1);
    }
 
    srand((unsigned)time(NULL));
 
    printf("=== Cwiczenie 5: Producent (potok nazwany FIFO) ===\n");
    printf("FIFO: %s  |  Zrodlo: %s  |  Porcja: %d B\n\n",
           argv[1], argv[2], PRODUCER_CHUNK);
 
    src_fd = open(argv[2], O_RDONLY);
    if (src_fd < 0)
        die("Producent: open (plik zrodlowy)");

    printf("[Producent] Czekam az Konsument otworzy FIFO...\n");
    fifo_fd = open(argv[1], O_WRONLY);
    if (fifo_fd < 0)
        die("Producent: open (FIFO)");
 
    printf("[Producent] Polaczono. Zaczynam wysylac dane.\n");
 
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
 
    printf("[Producent] Zakonczono. Wszystkie dane wyslane.\n");
    return 0;
}