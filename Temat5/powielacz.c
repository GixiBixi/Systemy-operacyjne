#define _POSIX_C_SOURCE 200809L
 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <signal.h>
 
#include "semafor.h"
#include "checkerror.h"
 
#define SEM_NAME   "/wykluczanie_sem"
#define NUMER_PLIK "numer.txt"
 
static const char *g_sem_name = SEM_NAME;
static sem_t      *g_sem      = NULL;
 
static void cleanup(void)
{
    if (g_sem != NULL) {
        sem_zamknij(g_sem);
        g_sem = NULL;
    }
    sem_unlink(g_sem_name);
    printf("[powielacz] Semafor '%s' usunięty.\n", g_sem_name);
}
 

static void obsluga_sigint(int signo)
{
    (void)signo;
    printf("\n[powielacz] Otrzymano SIGINT – kończenie...\n");
    exit(EXIT_SUCCESS);
}
 
static void utworz_plik_numer(void)
{
    FILE *f = fopen(NUMER_PLIK, "w");
    if (f == NULL) {
        perror("fopen numer.txt");
        exit(EXIT_FAILURE);
    }
    fprintf(f, "0\n");
    fclose(f);
    printf("[powielacz] Utworzono plik '%s' z wartością 0.\n", NUMER_PLIK);
}
 
int main(int argc, char *argv[])
{
    if (argc != 3) {
        fprintf(stderr, "Użycie: %s <liczba_procesow> <liczba_sekcji>\n", argv[0]);
        return EXIT_FAILURE;
    }
 
    int liczba_procesow = atoi(argv[1]);
    int liczba_sekcji   = atoi(argv[2]);
 
    if (liczba_procesow <= 0 || liczba_sekcji <= 0) {
        fprintf(stderr, "[powielacz] Argumenty muszą być dodatnimi liczbami całkowitymi.\n");
        return EXIT_FAILURE;
    }
 
    atexit(cleanup);
    signal(SIGINT, obsluga_sigint);
 
    sem_unlink(SEM_NAME);
 
    g_sem = sem_utworz(SEM_NAME, 1);
    CheckError(g_sem);
 
    int wartosc_pocz = -1;
    CheckError(sem_wartosc(g_sem, &wartosc_pocz));
    printf("[powielacz] Semafor '%s' utworzony.\n", SEM_NAME);
    printf("[powielacz]   adres:          %p\n", (void *)g_sem);
    printf("[powielacz]   wartość początkowa: %d\n", wartosc_pocz);
 
    utworz_plik_numer();
 
    printf("[powielacz] Uruchamiam %d proces(ów), każdy z %d sekcją/sekcjami krytyczną.\n\n",
           liczba_procesow, liczba_sekcji);
 
    char str_sekcji[16];
    snprintf(str_sekcji, sizeof(str_sekcji), "%d", liczba_sekcji);
 
    for (int i = 0; i < liczba_procesow; i++) {
        pid_t pid = fork();
        if (pid < 0) {
            perror("fork");
            exit(EXIT_FAILURE);
        }
        if (pid == 0) {
            execlp("./wykluczanie", "./wykluczanie", SEM_NAME, str_sekcji, NULL);
            perror("execlp");
            _exit(EXIT_FAILURE);
        }
    }
 
    int status;
    for (int i = 0; i < liczba_procesow; i++) {
        pid_t pid = wait(&status);
        if (pid < 0) {
            perror("wait");
        } else {
            printf("[powielacz] Proces PID=%d zakończył się z kodem %d.\n",
                   pid, WEXITSTATUS(status));
        }
    }
 
    FILE *f = fopen(NUMER_PLIK, "r");
    if (f == NULL) {
        perror("fopen numer.txt (odczyt końcowy)");
        exit(EXIT_FAILURE);
    }
    int numer_koncowy = 0;
    fscanf(f, "%d", &numer_koncowy);
    fclose(f);
 
    int oczekiwany = liczba_procesow * liczba_sekcji;
    printf("\n[powielacz] ================================================\n");
    printf("[powielacz] Końcowa wartość numeru:   %d\n", numer_koncowy);
    printf("[powielacz] Oczekiwana wartość:        %d\n", oczekiwany);
    if (numer_koncowy == oczekiwany) {
        printf("[powielacz] WYNIK: POPRAWNA synchronizacja procesów.\n");
    } else {
        printf("[powielacz] WYNIK: BŁĄD synchronizacji! Wyścig danych.\n");
    }
    printf("[powielacz] ================================================\n");
 
    return EXIT_SUCCESS;
}
 