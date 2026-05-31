#ifndef SEM_UTILS_H
#define SEM_UTILS_H
#include <semaphore.h>
sem_t *sem_create(const char *name, unsigned int init_val);
sem_t *sem_open_existing(const char *name);
void sem_print_value(const char *label, sem_t *sem);
void sem_remove(const char *name, sem_t *sem);
 
#endif /* SEM_UTILS_H */
 