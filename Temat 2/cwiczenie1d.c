/*
Jak w (b), tylko wstawi% funkcj! sleep w takich miejscach programu, aby procesy
pojawialy si! na ekranie grupowane pokoleniami od najstarszego do najmlodszego,
a proces macierzysty konczy sie dopiero po procesach potomnych (nie uzywac
funkcji wait). Na podstawie wyników programu sporz$dzi% „drzewo genealogiczne”
tworzonych procesów, tzn. do pliku tekstowego „wkleic" wyniki programu, a nastepnie 
narysowac drzewo genealogiczne, zaznaczajac na nim identyfikatory PID, PPID i PGID.
*/



#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

void print_ids(int generation)
{
    printf("[GEN %d] PID=%d PPID=%d PGID=%d\n",
           generation, (int)getpid(), (int)getppid(), (int)getpgrp());
    fflush(stdout);
}

int main(void)
{
    int i;
    int generation = 0; //zmienna sledzaca pokolenie procesu 0=rodzic, 1=potomek, 2=potomek potomka

    for (i = 0; i < 3; i++) {
        switch (fork()) {
            case -1:
                perror("fork error");
                exit(EXIT_FAILURE);
            case 0:
                generation = i + 1;
                break;
            default:
                break;
        }
    } //kazdy proces wykonuje tą pętle i powstaje drzewo, łącznie 8 procesów

    int sleep_time = (3 - generation) * 2;   //oblicza czas spania. im starsze pokolenie tym dłużej śpi: np. gen 0 --> 6 ...
    sleep(sleep_time); //dzieki temu pojawiaja sie najpierw najmlodsze potem rodzic i konczy sie ostatni
    print_ids(generation);

    return 0;
}



