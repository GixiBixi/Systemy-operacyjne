#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

#include "common.h"
#include "semafory.h"
#include "pamiec.h"

static const char *g_shm_name  = NULL;
static const char *g_sem_prod  = NULL;
static const char *g_sem_kons  = NULL;

static void sprzatanie_atexit(void);
static void handler_sigint(int sig);


int main(int argc, char *argv[])
{
    const char *shm_name  = (argc > 1) ? argv[1] : DEFAULT_SHM_NAME;
    const char *sem_prod  = (argc > 2) ? argv[2] : DEFAULT_SEM_PROD;
    const char *sem_kons  = (argc > 3) ? argv[3] : DEFAULT_SEM_KONS;
    const char *file_prod = (argc > 4) ? argv[4] : DEFAULT_FILE_PROD;
    const char *file_kons = (argc > 5) ? argv[5] : DEFAULT_FILE_KONS;

    g_shm_name = shm_name;
    g_sem_prod = sem_prod;
    g_sem_kons = sem_kons;

    printf("=== Proces macierzysty (PID=%d) ===\n", (int)getpid());
    printf("    shm  : %s\n", shm_name);
    printf("    PROD : %s\n", sem_prod);
    printf("    KONS : %s\n", sem_kons);
    printf("    plik producenta  : %s\n", file_prod);
    printf("    plik konsumenta  : %s\n", file_kons);
    printf("    NELE=%d  NBUF=%d\n\n", NELE, NBUF);

    atexit(sprzatanie_atexit);
    signal(SIGINT, handler_sigint);

    sem_t *sp = sem_utworz(sem_prod, (unsigned)NBUF);
    sem_t *sk = sem_utworz(sem_kons, 0u);

    int val_p = sem_wartosc(sp);
    int val_k = sem_wartosc(sk);
    printf("[main] Semafory utworzone:\n");
    printf("  PROD '%s'  adres=%p  wartosc=%d\n", sem_prod, (void*)sp, val_p);
    printf("  KONS '%s'  adres=%p  wartosc=%d\n\n", sem_kons, (void*)sk, val_k);

    sem_zamknij(sp, sem_prod);
    sem_zamknij(sk, sem_kons);

    int des = -1;
    size_t rozmiar = sizeof(SegmentPD);
    SegmentPD *wpd = shm_utworz(shm_name, rozmiar, &des);

    wpd->wstaw  = 0;
    wpd->wyjmij = 0;
    memset(wpd->bufor, 0, sizeof(wpd->bufor));

    printf("[main] Pamiec dzielona utworzona:\n");
    printf("  nazwa='%s'  deskryptor=%d  rozmiar=%zu bajtow\n\n",
           shm_name, des, rozmiar);

    shm_odwzoruj_usun(wpd, rozmiar);
    shm_zamknij(des, shm_name);

    pid_t pid_prod = fork();
    CheckError(pid_prod, "fork (producent)");

    if (pid_prod == 0) {
        execlp("./producent", "./producent",
               shm_name, sem_prod, sem_kons, file_prod, (char *)NULL);
        perror("execlp (producent)");
        exit(EXIT_FAILURE);
    }

    pid_t pid_kons = fork();
    CheckError(pid_kons, "fork (konsument)");

    if (pid_kons == 0) {
        execlp("./konsument", "./konsument",
               shm_name, sem_prod, sem_kons, file_kons, (char *)NULL);
        perror("execlp (konsument)");
        exit(EXIT_FAILURE);
    }

    printf("[main] Uruchomiono producenta (PID=%d) i konsumenta (PID=%d)\n\n",
           (int)pid_prod, (int)pid_kons);

    int status;
    waitpid(pid_prod, &status, 0);
    printf("[main] Producent (PID=%d) zakonczyl sie, status=%d\n",
           (int)pid_prod, WEXITSTATUS(status));

    waitpid(pid_kons, &status, 0);
    printf("[main] Konsument (PID=%d) zakonczyl sie, status=%d\n",
           (int)pid_kons, WEXITSTATUS(status));

    printf("\n[main] Usuwanie zasobow...\n");
    return EXIT_SUCCESS;
}

static void sprzatanie_atexit(void)
{
    if (g_sem_prod) sem_usun(g_sem_prod);
    if (g_sem_kons) sem_usun(g_sem_kons);
    if (g_shm_name) shm_usun(g_shm_name);
    printf("[main] Zasoby usuniete.\n");
}

static void handler_sigint(int sig)
{
    (void)sig;
    printf("\n[main] Odebrano SIGINT – sprzatanie...\n");
    exit(EXIT_FAILURE);
}
