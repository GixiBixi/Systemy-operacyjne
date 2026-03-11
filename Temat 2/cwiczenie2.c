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

int main(int argc, char *argv[])
{
    if (argc < 2) {
        fprintf(stderr, "Użycie: %s <ścieżka_do_child_printer>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    const char *child_prog = argv[1];

    print_ids("RODZIC (przed fork)");

    for (int i = 0; i < 3; i++) {
        switch (fork()) {
            case -1:
                perror("fork error");
                exit(EXIT_FAILURE);

            case 0: {
                char label[32];
                snprintf(label, sizeof(label), "POTOMEK_%d", i + 1);

                execlp(child_prog, child_prog, label, (char *)NULL);

                perror("execlp error");
                _exit(EXIT_FAILURE);
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