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
    int generation = 0;   /* 0 = rodzic pierwotny */

    for (i = 0; i < 3; i++) {
        switch (fork()) {
            case -1:
                perror("fork error");
                exit(EXIT_FAILURE);
            case 0:
                /* Nowy potomek – zapamiętaj swoje pokolenie i kontynuuj pętlę */
                generation = i + 1;
                break;
            default:
                /* Rodzic kontynuuje pętlę */
                break;
        }
    }

    /*
     * Każdy proces (rodzic i potomkowie) dochodzi tu po pętli.
     * Czas spania odwrotnie proporcjonalny do pokolenia →
     * najstarszy drukuje ostatni.
     */
    int sleep_time = (3 - generation) * 2;   /* gen0→6s, gen1→4s, gen2→2s, gen3→0s */
    sleep(sleep_time);
    print_ids(generation);

    return 0;
}



