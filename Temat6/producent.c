#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>

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

    printf("\n=== Producent (PID=%d) ===\n", (int)getpid());
    printf("    plik zrodlowy: %s\n", plik);

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
    int fd = open(plik, O_RDONLY);
    CheckError(fd, "open (producent)");

    int nr_porcji = 0;
    ssize_t n;
    Towar t;

    while (1) {
        memset(t.element, 0, NELE);
        n = read(fd, t.element, NELE - 1);
        CheckError((int)n, "read (producent)");

        int koniec = (n == 0);

        if (koniec) {
            memset(t.element, 0, NELE);
        }

        sem_P(sp, sem_prod);

        wpd->bufor[wpd->wstaw] = t;
        int idx = wpd->wstaw;
        wpd->wstaw = (wpd->wstaw + 1) % NBUF;

        int vp = sem_wartosc(sp);
        int vk = sem_wartosc(sk);

        printf("[PROD] porcja=%d  idx=%d  bajtow=%zd  PROD=%d KONS=%d  dane: \"",
               ++nr_porcji, idx, (koniec ? 0 : n), vp, vk);
        if (!koniec) {
            fwrite(t.element, 1, (size_t)n, stdout);
        }
        printf("\"%s\n", koniec ? "  [KONIEC]" : "");
        fflush(stdout);

        sem_V(sk, sem_kons);

        if (koniec) break;
    }

    CheckError(close(fd), "close fd (producent)");
    shm_odwzoruj_usun(wpd, rozmiar);
    shm_zamknij(des, shm_name);
    sem_zamknij(sp, sem_prod);
    sem_zamknij(sk, sem_kons);

    printf("[PROD] Zakonczono. Wyslano %d porcji (+1 koniec).\n", nr_porcji - 1);
    return EXIT_SUCCESS;
}
