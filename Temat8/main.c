#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
 
#include "buffer.h"
#include "sem_utils.h"
#include "shm_utils.h"

#define SEM_EMPTY_NAME  "/sem_empty_zad9"
#define SEM_FULL_NAME   "/sem_full_zad9"
#define SEM_MUTEX_NAME  "/sem_mutex_zad9"
#define SHM_NAME        "/shm_zad9"
#define PROD_FILE       "producer_data.txt"
#define CONS_FILE       "consumer_data.txt"

static void create_producer_file(void)
{
    const char text[] =
        "Ala ma kota, kot ma Ale. "
        "Bufor cykliczny dziala poprawnie! "
        "Producent wysyla dane, konsument je odbiera. "
        "Wzajemne wykluczanie gwarantuja semafory POSIX.";
 
    FILE *f = fopen(PROD_FILE, "w");
    if (!f) { perror("fopen (tworzenie pliku producenta)"); exit(EXIT_FAILURE); }
    fputs(text, f);
    fclose(f);
    printf("[MAIN] Utworzono plik producenta '%s' (%zu B).\n",
           PROD_FILE, strlen(text));
}

int main(void)
{
    printf("============================================================\n");
    printf(" Zadanie 9 – Producent/Konsument z buforem cyklicznym\n");
    printf(" Semafory POSIX + pamięć dzielona POSIX\n");
    printf(" PID procesu macierzystego: %d\n", (int)getpid());
    printf("============================================================\n\n");
 
    create_producer_file();
 
    sem_t *sem_empty = sem_create(SEM_EMPTY_NAME, BUF_COUNT);
    sem_t *sem_full  = sem_create(SEM_FULL_NAME,  0);
    sem_t *sem_mutex = sem_create(SEM_MUTEX_NAME, 1);
 
    int shm_fd;
    SharedBuf *buf = shm_create(SHM_NAME, SHM_SIZE, &shm_fd);

    buf->in  = 0;
    buf->out = 0;
    memset(buf->data, 0, sizeof(buf->data));
 
    printf("\n[MAIN] Bufor cykliczny: %d elementów x %d B = %zu B w pamięci dzielonej.\n\n",
           BUF_COUNT, BUF_ELEM_SIZE, SHM_SIZE);

    pid_t pid_prod = fork();
    if (pid_prod < 0) {
        perror("fork (producent)");
        exit(EXIT_FAILURE);
    }
    if (pid_prod == 0) {

        execlp("./producer", "producer",
               SEM_EMPTY_NAME, SEM_FULL_NAME, SEM_MUTEX_NAME,
               SHM_NAME, PROD_FILE, (char *)NULL);
        perror("execlp (producer)");
        exit(EXIT_FAILURE);
    }
 
    pid_t pid_cons = fork();
    if (pid_cons < 0) {
        perror("fork (konsument)");
        exit(EXIT_FAILURE);
    }
    if (pid_cons == 0) {
        execlp("./consumer", "consumer",
               SEM_EMPTY_NAME, SEM_FULL_NAME, SEM_MUTEX_NAME,
               SHM_NAME, CONS_FILE, (char *)NULL);
        perror("execlp (consumer)");
        exit(EXIT_FAILURE);
    }
 
    printf("[MAIN] Uruchomiono producenta (PID=%d) i konsumenta (PID=%d).\n",
           (int)pid_prod, (int)pid_cons);
 
    int status;
    waitpid(pid_prod, &status, 0);
    printf("[MAIN] Producent (PID=%d) zakończył: status=%d\n",
           (int)pid_prod, WEXITSTATUS(status));
 
    waitpid(pid_cons, &status, 0);
    printf("[MAIN] Konsument (PID=%d) zakończył: status=%d\n",
           (int)pid_cons, WEXITSTATUS(status));
 
    printf("\n[MAIN] Usuwam zasoby IPC...\n");
    sem_remove(SEM_EMPTY_NAME, sem_empty);
    sem_remove(SEM_FULL_NAME,  sem_full);
    sem_remove(SEM_MUTEX_NAME, sem_mutex);
    shm_remove(SHM_NAME, buf, SHM_SIZE, shm_fd);
 
    printf("\n[MAIN] Porównanie plików '%s' i '%s':\n", PROD_FILE, CONS_FILE);
    fflush(stdout);
 
    return EXIT_SUCCESS;
}
 