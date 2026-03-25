/*
Jak w (b), tylko przy uzyciu fukcji setpgid sprawic by kazdy proces potomny
stawac sie liderem swojej wlasnej grupy procesów.
Ile procesów powstanie przy n-krotnym wywo#aniu funkcji fork i dlaczego?
*/



#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

void print_ids(const char *label)
{
    printf("[%s] PID=%d PPID=%d PGID=%d\n",
           label, (int)getpid(), (int)getppid(), (int)getpgrp());
    fflush(stdout);
}

int main(void)
{
    int i;

    print_ids("RODZIC (przed fork)");

    for (i = 0; i < 3; i++) {
        switch (fork()) {
            case -1:
                perror("fork error");
                exit(EXIT_FAILURE);
            case 0: {
                if (setpgid(0, 0) == -1) { //setpig(0,0) pierwsze zero dotyczy biezacego procesu(potomka), drugie zero -> ustaw PGID rowny jego własnemu PID staje sie liderem nowej grupy
                    //efekt jest taki że każdy potomek ma własną grupe procesów, oddzieloną od rodzica, widac to w wynikach -PGID potomka == jego wlasny PID
                    perror("setpgid error");
                    _exit(EXIT_FAILURE);
                }
                char label[32];
                snprintf(label, sizeof(label), "POTOMEK %d", i + 1);
                print_ids(label);
                _exit(0);
            }
            default:
                break;
        }
    }

    int status;
    while (wait(&status) > 0)
        ;

    print_ids("RODZIC (po wait)");
    return 0;
}