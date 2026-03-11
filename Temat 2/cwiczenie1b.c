#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
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

    print_ids("RODZIC (przed fork)");

    for (i = 0; i < 3; i++) {
        pid = fork();
        switch (pid) {
            case -1:
                perror("fork error");
                exit(EXIT_FAILURE);
            case 0:
                /* proces potomny – wypisz i wyjdź (bez exit/return) */
                {
                    char label[32];
                    snprintf(label, sizeof(label), "POTOMEK %d", i + 1);
                    print_ids(label);
                    /* zgodnie z treścią: nie używamy exit ani return w potomku */
                    _exit(0);
                }
            default:
                /* proces macierzysty kontynuuje pętlę */
                break;
        }
    }

    /* Czekaj na wszystkie 3 procesy potomne */
    int status;
    while (wait(&status) > 0)
        ;

    print_ids("RODZIC (po wait)");
    return 0;
}
