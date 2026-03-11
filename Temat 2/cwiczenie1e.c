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
                /* Potomek staje się liderem własnej grupy procesów */
                if (setpgid(0, 0) == -1) {
                    perror("setpgid error");
                    _exit(EXIT_FAILURE);
                }
                char label[32];
                snprintf(label, sizeof(label), "POTOMEK %d", i + 1);
                print_ids(label);
                /* PGID potomka == jego własny PID */
                _exit(0);
            }
            default:
                break;
        }
    }

    /* Czekaj na wszystkie procesy potomne */
    int status;
    while (wait(&status) > 0)
        ;

    print_ids("RODZIC (po wait)");
    return 0;