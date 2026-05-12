#define _POSIX_C_SOURCE 200809L
 
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>
 
#include "semafor.h"
#include "checkerror.h"
 
#define NUMER_PLIK "numer.txt"

static int czytaj_numer(void)
{
    FILE *f = fopen(NUMER_PLIK, "r");
    if (f == NULL) {
        perror("fopen (odczyt)");
        exit(EXIT_FAILURE);
    }
    int n = 0;
    if (fscanf(f, "%d", &n) != 1) {
        fprintf(stderr, "[wykluczanie] Błąd odczytu numeru z pliku.\n");
        fclose(f);
        exit(EXIT_FAILURE);
    }
    fclose(f);
    return n;
}

static void zapisz_numer(int n)
{
    FILE *f = fopen(NUMER_PLIK, "w");
    if (f == NULL) {
        perror("fopen (zapis)");
        exit(EXIT_FAILURE);
    }
    fprintf(f, "%d\n", n);
    fclose(f);
}

int main(int argc, char *argv[])
{
    if (argc != 3) {
        fprintf(stderr, "Użycie: %s <nazwa_semafora> <liczba_sekcji>\n", argv[0]);
        return EXIT_FAILURE;
    }
 
    const char *sem_name    = argv[1];
    int         liczba_sekcji = atoi(argv[2]);
    pid_t       pid           = getpid();

    srand((unsigned int)(time(NULL) ^ pid));

    sem_t *sem = sem_otworz(sem_name);
    CheckError(sem);
 
    printf("[PID=%d] Uruchomiony. Semafor '%s' otwarty.\n", pid, sem_name);
 
    for (int i = 1; i <= liczba_sekcji; i++) {

        int t_prywatna = 1 + rand() % 3;
        int wartosc = -1;
        CheckError(sem_wartosc(sem, &wartosc));
        printf("[PID=%d] Sekcja %d: sekcja PRYWATNA (sleep %ds), sem=%d\n",
               pid, i, t_prywatna, wartosc);
        sleep(t_prywatna);

        CheckError(sem_wartosc(sem, &wartosc));
        printf("[PID=%d] Sekcja %d: czekam na wejście do sekcji krytycznej, sem=%d\n",
               pid, i, wartosc);
 
        CheckError(sem_opusc(sem));
 
        CheckError(sem_wartosc(sem, &wartosc));
        printf("[PID=%d] Sekcja %d: WESZŁEM do sekcji krytycznej, sem=%d\n",
               pid, i, wartosc);

        int numer = czytaj_numer();
        printf("\t[PID=%d] Sekcja %d: [SK] odczytano numer = %d\n", pid, i, numer);

        int t_sk = 1 + rand() % 3;
        printf("\t[PID=%d] Sekcja %d: [SK] sleep %ds...\n", pid, i, t_sk);
        sleep(t_sk);
 
        numer++;
        zapisz_numer(numer);
        printf("\t[PID=%d] Sekcja %d: [SK] zapisano numer = %d\n", pid, i, numer);

        CheckError(sem_podniesc(sem));
 
        CheckError(sem_wartosc(sem, &wartosc));
        printf("[PID=%d] Sekcja %d: WYSZEDŁEM z sekcji krytycznej, sem=%d\n",
               pid, i, wartosc);
    }
 
    printf("[PID=%d] Wszystkie sekcje krytyczne wykonane. Kończę.\n", pid);
 
    CheckError(sem_zamknij(sem));
 
    return EXIT_SUCCESS;
}