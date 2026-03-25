/*
Jak w (b), tylko przy u’yciu funkcji sleep (nie uzywac funkcji wait) sprawiac by procesy potomne by&y adoptowane przez proces init lub jego odpowiednik.
Nastepnie z poziomu powloki wykonac polecenie pstree -p i w wyswietlonym
drzewie procesów zidentyfikowa% proces, który adoptuje osierocone procesy w danym systemie.
*/


#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

void print_ids(const char *label)
{
    printf("[%s] UID=%d GID=%d PID=%d PPID=%d PGID=%d\n",
           label,
           (int)getuid(), (int)getgid(),
           (int)getpid(), (int)getppid(), (int)getpgrp());
    fflush(stdout);
}

int main(void)
{
    int i;
    pid_t pid;

    for (i = 0; i < 3; i++) { //petla wykonuje fork trzy razy, tworzac trzech potomkow
        pid = fork();
        switch (pid) {
            case -1:
                perror("fork error");
                exit(EXIT_FAILURE);
            case 0:
                sleep(10); //potomek spi 10 sekund, chodzi o to zeby rodzic zdarzyl sie zakończyć zanim potomek się obudzi
                print_ids("POTOMEK (po uśpieniu – sprawdź PPID)");
                _exit(0);
            default:
                break;
        }
    }

    print_ids("RODZIC (kończy się)");
    return 0;
}