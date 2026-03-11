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

    for (i = 0; i < 3; i++) {
        pid = fork();
        switch (pid) {
            case -1:
                perror("fork error");
                exit(EXIT_FAILURE);
            case 0:
                /* Potomek śpi długo – rodzic zdąży zakończyć się wcześniej.
                 * Po zakończeniu rodzica, potomek zostanie adoptowany przez
                 * init (PID=1) lub systemd (PID może być > 1).
                 * Sprawdź to poleceniem:  pstree -p
                 */
                sleep(10);
                print_ids("POTOMEK (po uśpieniu – sprawdź PPID)");
                _exit(0);
            default:
                break;
        }
    }

    /* Rodzic kończy się natychmiast – nie czeka (brak wait) */
    print_ids("RODZIC (kończy się)");
    return 0;
}