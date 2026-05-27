#include "semafory.h"
#include "common.h"
#include <fcntl.h>
#include <sys/stat.h>

sem_t *sem_utworz(const char *nazwa, unsigned int wartosc)
{
    sem_t *s = sem_open(nazwa, O_CREAT | O_EXCL | O_RDWR,
                        S_IRUSR | S_IWUSR, wartosc);
    if (s == SEM_FAILED) {
        perror("sem_open (tworzenie)");
        fprintf(stderr, "  nazwa semafora: %s\n", nazwa);
        exit(EXIT_FAILURE);
    }
    return s;
}

sem_t *sem_otworz(const char *nazwa)
{
    sem_t *s = sem_open(nazwa, 0);
    if (s == SEM_FAILED) {
        perror("sem_open (otwieranie)");
        fprintf(stderr, "  nazwa semafora: %s\n", nazwa);
        exit(EXIT_FAILURE);
    }
    return s;
}

void sem_P(sem_t *sem, const char *nazwa)
{
    (void)nazwa;
    if (sem_wait(sem) == -1) {
        perror("sem_wait");
        exit(EXIT_FAILURE);
    }
}

void sem_V(sem_t *sem, const char *nazwa)
{
    (void)nazwa;
    if (sem_post(sem) == -1) {
        perror("sem_post");
        exit(EXIT_FAILURE);
    }
}

void sem_zamknij(sem_t *sem, const char *nazwa)
{
    if (sem_close(sem) == -1) {
        perror("sem_close");
        fprintf(stderr, "  nazwa semafora: %s\n", nazwa);
    }
}

void sem_usun(const char *nazwa)
{
    if (sem_unlink(nazwa) == -1) {
        perror("sem_unlink");
        fprintf(stderr, "  nazwa semafora: %s\n", nazwa);
    }
}

int sem_wartosc(sem_t *sem)
{
    int val = -1;
    if (sem_getvalue(sem, &val) == -1) {
        perror("sem_getvalue");
    }
    return val;
}
