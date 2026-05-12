#ifndef SEMAFOR_H
#define SEMAFOR_H
#include <semaphore.h>

sem_t *sem_utworz(const char *name, unsigned int value);
sem_t *sem_otworz(const char *name);
int sem_wartosc(sem_t *sem, int *wartosc);
int sem_opusc(sem_t *sem);
int sem_podniesc(sem_t *sem);
int sem_zamknij(sem_t *sem);
int sem_usun(const char *name);
 
#endif /* SEMAFOR_H */