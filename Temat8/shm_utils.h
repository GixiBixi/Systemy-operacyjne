#ifndef SHM_UTILS_H
#define SHM_UTILS_H
#include <stddef.h>
void *shm_create(const char *name, size_t size, int *out_fd);
void *shm_open_existing(const char *name, size_t size, int *out_fd);
void shm_remove(const char *name, void *addr, size_t size, int fd);
 
#endif /* SHM_UTILS_H */
 