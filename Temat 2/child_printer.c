#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    const char *label = (argc > 1) ? argv[1] : "POTOMEK";
    printf("[%s] UID=%d GID=%d PID=%d PPID=%d PGID=%d\n",
           label,
           (int)getuid(), (int)getgid(),
           (int)getpid(), (int)getppid(), (int)getpgrp());
    fflush(stdout);
    return 0;
}

// Program robi tylko dwie rzeczy
// - odbiera etykiete jako argument (np. "POTOMEK 1")
// - wypisuje identyfikatory procesu