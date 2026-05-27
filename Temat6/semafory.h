#ifndef SEMAFORY_H
#define SEMAFORY_H
#include <semaphore.h>
sem_t *sem_utworz(const char *nazwa, unsigned int wartosc);
sem_t *sem_otworz(const char *nazwa);
void sem_P(sem_t *sem, const char *nazwa);
void sem_V(sem_t *sem, const char *nazwa);
void sem_zamknij(sem_t *sem, const char *nazwa);
void sem_usun(const char *nazwa);
int sem_wartosc(sem_t *sem);

#endif
