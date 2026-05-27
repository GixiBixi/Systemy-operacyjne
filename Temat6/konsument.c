#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>

#include "common.h"
#include "semafory.h"
#include "pamiec.h"

int main(int argc, char *argv[])
{
    if (argc < 5) {
        fprintf(stderr, "Uzycie: %s shm_name sem_prod sem_kons plik\n",
                argv[0]);
        exit(EXIT_FAILURE);
    }

    const char *shm_name = argv[1];
    const char *sem_prod = argv[2];
    const char *sem_kons = argv[3];
    const char *plik     = argv[4];

    printf("\n=== Konsument (PID=%d) ===\n", (int)getpid());
    printf("    plik docelowy: %s\n", plik);

    sem_t *sp = sem_otworz(sem_prod);
    sem_t *sk = sem_otworz(sem_kons);

    printf("    Semafor PROD '%s' adres=%p wartosc=%d\n",
           sem_prod, (void*)sp, sem_wartosc(sp));
    printf("    Semafor KONS '%s' adres=%p wartosc=%d\n",
           sem_kons, (void*)sk, sem_wartosc(sk));

    int des = -1;
    size_t rozmiar = sizeof(SegmentPD);
    SegmentPD *wpd = shm_otworz(shm_name, rozmiar,
                                PROT_READ | PROT_WRITE, &des);

    printf("    Pamiec dzielona '%s' deskryptor=%d\n\n", shm_name, des);

    int fd = open(plik, O_WRONLY | O_CREAT | O_TRUNC,
                  S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    CheckError(fd, "open (konsument)");

    int nr_porcji = 0;

    while (1) {
        sem_P(sk, sem_kons);

        Towar t = wpd->bufor[wpd->wyjmij];
        int idx  = wpd->wyjmij;
        wpd->wyjmij = (wpd->wyjmij + 1) % NBUF;

        sem_V(sp, sem_prod);

        if (t.element[0] == '\0') {
            int vp = sem_wartosc(sp);
            int vk = sem_wartosc(sk);
            printf("[KONS] porcja=%d  idx=%d  bajtow=0  PROD=%d KONS=%d  "
                   "[KONIEC]\n", ++nr_porcji, idx, vp, vk);
            fflush(stdout);
            break;
        }

        size_t dlugosc = strnlen(t.element, NELE - 1);

        int vp = sem_wartosc(sp);
        int vk = sem_wartosc(sk);
        printf("[KONS] porcja=%d  idx=%d  bajtow=%zu  PROD=%d KONS=%d  dane: \"",
               ++nr_porcji, idx, dlugosc, vp, vk);
        fwrite(t.element, 1, dlugosc, stdout);
        printf("\"\n");
        fflush(stdout);

        ssize_t wn = write(fd, t.element, dlugosc);
        CheckError((int)wn, "write (konsument)");
    }

    CheckError(close(fd), "close fd (konsument)");
    shm_odwzoruj_usun(wpd, rozmiar);
    shm_zamknij(des, shm_name);
    sem_zamknij(sp, sem_prod);
    sem_zamknij(sk, sem_kons);

    printf("[KONS] Zakonczono. Odebrano %d porcji.\n", nr_porcji - 1);
    return EXIT_SUCCESS;
}
