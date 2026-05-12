#include "semafor.h"
 
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>

#define SEM_MODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)
 
sem_t *sem_utworz(const char *name, unsigned int value)
{
    sem_t *sem = sem_open(name, O_CREAT | O_EXCL, SEM_MODE, value);
    if (sem == SEM_FAILED) {
        perror("sem_open (O_CREAT|O_EXCL)");
        return NULL;
    }
    return sem;
}
 
sem_t *sem_otworz(const char *name)
{
    sem_t *sem = sem_open(name, 0);
    if (sem == SEM_FAILED) {
        perror("sem_open");
        return NULL;
    }
    return sem;
}
 
int sem_wartosc(sem_t *sem, int *wartosc)
{
    if (sem_getvalue(sem, wartosc) == -1) {
        perror("sem_getvalue");
        return 0;
    }
    return 1;
}
 
int sem_opusc(sem_t *sem)
{
    if (sem_wait(sem) == -1) {
        perror("sem_wait");
        return 0;
    }
    return 1;
}
 
int sem_podniesc(sem_t *sem)
{
    if (sem_post(sem) == -1) {
        perror("sem_post");
        return 0;
    }
    return 1;
}
 
int sem_zamknij(sem_t *sem)
{
    if (sem_close(sem) == -1) {
        perror("sem_close");
        return 0;
    }
    return 1;
}
 
int sem_usun(const char *name)
{
    if (sem_unlink(name) == -1) {
        perror("sem_unlink");
        return 0;
    }
    return 1;
}