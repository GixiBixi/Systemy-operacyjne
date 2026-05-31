#define _REENTRANT
#include "sem_utils.h"
 
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
 
sem_t *sem_create(const char *name, unsigned int init_val)
{
    sem_t *sem = sem_open(name, O_CREAT | O_EXCL, 0600, init_val);
    if (sem == SEM_FAILED) {
        perror("sem_open (create)");
        exit(EXIT_FAILURE);
    }
    printf("[SEM] Utworzono semafor '%s': adres=%p, wartość początkowa=%u\n",
           name, (void *)sem, init_val);
    return sem;
}
 
sem_t *sem_open_existing(const char *name)
{
    sem_t *sem = sem_open(name, 0);
    if (sem == SEM_FAILED) {
        perror("sem_open (open)");
        exit(EXIT_FAILURE);
    }
    printf("[SEM] Otwarto semafor '%s': adres=%p\n", name, (void *)sem);
    return sem;
}
 
void sem_print_value(const char *label, sem_t *sem)
{
    int val = 0;
    if (sem_getvalue(sem, &val) == -1) {
        perror("sem_getvalue");
        return;
    }
    printf("[SEM] %s = %d\n", label, val);
}
 
void sem_remove(const char *name, sem_t *sem)
{
    if (sem_close(sem) == -1) {
        perror("sem_close");
        exit(EXIT_FAILURE);
    }
    if (sem_unlink(name) == -1) {
        perror("sem_unlink");
        exit(EXIT_FAILURE);
    }
    printf("[SEM] Usunięto semafor '%s'\n", name);
}