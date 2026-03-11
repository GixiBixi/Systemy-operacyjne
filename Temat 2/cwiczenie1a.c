#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

int main(void)
{
    printf("UID  = %d\n", (int)getuid());
    printf("GID  = %d\n", (int)getgid());
    printf("PID  = %d\n", (int)getpid());
    printf("PPID = %d\n", (int)getppid());
    printf("PGID = %d\n", (int)getpgrp());
    return 0;
}
