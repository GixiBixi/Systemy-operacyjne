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
    int fifo_fd, dst_fd;
    ssize_t n;
    char buf[CONSUMER_CHUNK];
 
    if (argc != 3) {
        fprintf(stderr, "Uzycie: %s sciezka_fifo plik_docelowy\n", argv[0]);
        exit(1);
    }
 
    srand((unsigned)time(NULL));
 
    printf("=== Cwiczenie 5: Konsument (potok nazwany FIFO) ===\n");
    printf("FIFO: %s  |  Cel: %s  |  Porcja: %d B\n\n",
           argv[1], argv[2], CONSUMER_CHUNK);
 
    dst_fd = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (dst_fd < 0)
        die("Konsument: open (plik docelowy)");
 
    printf("[Konsument] Czekam az Producent otworzy FIFO...\n");
    fifo_fd = open(argv[1], O_RDONLY);
    if (fifo_fd < 0)
        die("Konsument: open (FIFO)");
 
    printf("[Konsument] Polaczono. Zaczynam odbierac dane.\n");
 
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
 
    printf("[Konsument] Zakonczono. Wszystkie dane zapisane.\n");
    return 0;
}