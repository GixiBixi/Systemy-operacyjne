#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
 
#include "buffer.h"
#include "sem_utils.h"
#include "shm_utils.h"
 
int main(int argc, char *argv[])
{
    if (argc != 6) {
        fprintf(stderr, "Użycie: %s <sem_empty> <sem_full> <sem_mutex> "
                "<shm_name> <plik_wyjściowy>\n", argv[0]);
        return EXIT_FAILURE;
    }
 
    const char *sem_empty_name = argv[1];
    const char *sem_full_name  = argv[2];
    const char *sem_mutex_name = argv[3];
    const char *shm_name       = argv[4];
    const char *outfile        = argv[5];
 
    printf("\n=== KONSUMENT (PID=%d) START ===\n", (int)getpid());
 
    sem_t *sem_empty = sem_open_existing(sem_empty_name);
    sem_t *sem_full  = sem_open_existing(sem_full_name);
    sem_t *sem_mutex = sem_open_existing(sem_mutex_name);
 
    printf("[KONS] Adresy semaforów: sem_empty=%p  sem_full=%p  sem_mutex=%p\n",
           (void *)sem_empty, (void *)sem_full, (void *)sem_mutex);
 
    int shm_fd;
    SharedBuf *buf = shm_open_existing(shm_name, SHM_SIZE, &shm_fd);
    printf("[KONS] Deskryptor pamięci dzielonej: %d\n", shm_fd);
 
    int fd = open(outfile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd == -1) {
        perror("open (konsument)");
        exit(EXIT_FAILURE);
    }
 
    printf("[KONS] Pobieram dane z bufora i zapisuję do pliku '%s'...\n", outfile);
 
    char elem[BUF_ELEM_SIZE];
 
    for (;;) {
        if (sem_wait(sem_full)  == -1) { perror("sem_wait full");  exit(EXIT_FAILURE); }
        if (sem_wait(sem_mutex) == -1) { perror("sem_wait mutex"); exit(EXIT_FAILURE); }
 
        int idx = buf->out;
        memcpy(elem, buf->data[idx], BUF_ELEM_SIZE);
        buf->out = (idx + 1) % BUF_COUNT;
 
        sem_print_value("sem_full  (po P)", sem_full);
        sem_print_value("sem_empty (przed V)", sem_empty);
        sem_print_value("sem_mutex (zajęty=0)", sem_mutex);
 
        if (sem_post(sem_mutex) == -1) { perror("sem_post mutex"); exit(EXIT_FAILURE); }
        if (sem_post(sem_empty) == -1) { perror("sem_post empty"); exit(EXIT_FAILURE); }
 
        if (elem[0] == '\0') {
            printf("[KONS] idx=%d  <-- odebrany marker końca danych\n", idx);
            break;
        }

        size_t len = strnlen(elem, BUF_ELEM_SIZE - 1);
        printf("[KONS] idx=%d  bajty=%zu  dane=\"%.*s\"\n",
               idx, len, (int)len, elem);
 
        if (write(fd, elem, len) == -1) {
            perror("write (konsument)");
            exit(EXIT_FAILURE);
        }
    }
 
    close(fd);
 
    if (sem_close(sem_empty) == -1) { perror("sem_close"); exit(EXIT_FAILURE); }
    if (sem_close(sem_full)  == -1) { perror("sem_close"); exit(EXIT_FAILURE); }
    if (sem_close(sem_mutex) == -1) { perror("sem_close"); exit(EXIT_FAILURE); }
 
    printf("=== KONSUMENT (PID=%d) KONIEC ===\n\n", (int)getpid());
    return EXIT_SUCCESS;
}
 