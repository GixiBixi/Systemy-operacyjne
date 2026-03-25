/*Napisac program wypisujacy identyfikatory podane w tabeli 1 dla danego procesu.*/

#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
// dołączają biblioteki systemowe potrzebne do fork(), getpt(), sleep() itp.

int main(void)
{
    printf("UID  = %d\n", (int)getuid());
    printf("GID  = %d\n", (int)getgid());
    printf("PID  = %d\n", (int)getpid());
    printf("PPID = %d\n", (int)getppid());
    printf("PGID = %d\n", (int)getpgrp());
    return 0;
}

//IDENTYFIKATORY ZWIĄZANE Z PROCESAMI:
// UID - identyfikator użytkownika (rzeczywisty)
// GID - identyfikator grupy użytkownika (rzeczywisty)
// PID - identyfikator procesu
// PPID - identyfikator procesu macierzystego (przodka)
// PGID - identyfikator grupy procesów (= PID lidera grupy)