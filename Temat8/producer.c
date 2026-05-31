#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
 
#include "buffer.h"
#include "sem_utils.h"
#include "shm_utils.h"
 
int main(int argc, char *argv[])
{
    if (argc != 6) {
        fprintf(stderr, "Użycie: %s <sem_empty> <sem_full> <sem_mutex> "
                "<shm_name> <plik_wejściowy>\n", argv[0]);
        return EXIT_FAILURE;
    }
 
    const char *sem_empty_name = argv[1];
    const char *sem_full_name  = argv[2];
    const char *sem_mutex_name = argv[3];
    const char *shm_name       = argv[4];
    const char *infile         = argv[5];
 
    printf("\n=== PRODUCENT (PID=%d) START ===\n", (int)getpid());

    sem_t *sem_empty = sem_open_existing(sem_empty_name);
    sem_t *sem_full  = sem_open_existing(sem_full_name);
    sem_t *sem_mutex = sem_open_existing(sem_mutex_name);
 
    printf("[PROD] Adresy semaforów: sem_empty=%p  sem_full=%p  sem_mutex=%p\n",
           (void *)sem_empty, (void *)sem_full, (void *)sem_mutex);

    int shm_fd;
    SharedBuf *buf = shm_open_existing(shm_name, SHM_SIZE, &shm_fd);
    printf("[PROD] Deskryptor pamięci dzielonej: %d\n", shm_fd);

    int fd = open(infile, O_RDONLY);
    if (fd == -1) {
        perror("open (producent)");
        exit(EXIT_FAILURE);
    }
 
    printf("[PROD] Czytam dane z pliku '%s' i wstawiam do bufora "
           "(%d elementów x %d B)...\n", infile, BUF_COUNT, BUF_ELEM_SIZE);
 
    char elem[BUF_ELEM_SIZE];
    ssize_t n;
    int eof_sent = 0;
 
    while (!eof_sent) {
        memset(elem, 0, BUF_ELEM_SIZE);
        n = read(fd, elem, BUF_ELEM_SIZE - 1);
        if (n < 0) {
            perror("read (producent)");
            exit(EXIT_FAILURE);
        }
        if (n == 0) {
            memset(elem, 0, BUF_ELEM_SIZE);
            eof_sent = 1;
        }

        if (sem_wait(sem_empty) == -1) { perror("sem_wait empty"); exit(EXIT_FAILURE); }
        if (sem_wait(sem_mutex) == -1) { perror("sem_wait mutex"); exit(EXIT_FAILURE); }

        int idx = buf->in;
        memcpy(buf->data[idx], elem, BUF_ELEM_SIZE);
        buf->in = (idx + 1) % BUF_COUNT;
 
        sem_print_value("sem_empty (po P)", sem_empty);
        sem_print_value("sem_full  (przed V)", sem_full);
        sem_print_value("sem_mutex (zajęty=0)", sem_mutex);
        printf("[PROD] idx=%d  bajty=%zd  dane=\"%s\"%s\n",
               idx, n == 0 ? (ssize_t)1 : n, eof_sent ? "(EOF)" : elem,
               eof_sent ? "  <-- marker końca" : "");
 
        if (sem_post(sem_mutex) == -1) { perror("sem_post mutex"); exit(EXIT_FAILURE); }
        if (sem_post(sem_full)  == -1) { perror("sem_post full");  exit(EXIT_FAILURE); }
    }
 
    close(fd);
 
    if (sem_close(sem_empty) == -1) { perror("sem_close"); exit(EXIT_FAILURE); }
    if (sem_close(sem_full)  == -1) { perror("sem_close"); exit(EXIT_FAILURE); }
    if (sem_close(sem_mutex) == -1) { perror("sem_close"); exit(EXIT_FAILURE); }
 
    printf("=== PRODUCENT (PID=%d) KONIEC ===\n\n", (int)getpid());
    return EXIT_SUCCESS;
}
 