#define _GNU_SOURCE
#include "shm_utils.h"
 
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
 
void *shm_create(const char *name, size_t size, int *out_fd)
{
    int fd = shm_open(name, O_CREAT | O_EXCL | O_RDWR, 0600);
    if (fd == -1) {
        perror("shm_open (create)");
        exit(EXIT_FAILURE);
    }
    if (ftruncate(fd, (off_t)size) == -1) {
        perror("ftruncate");
        exit(EXIT_FAILURE);
    }
    void *addr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (addr == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }
    printf("[SHM] Utworzono pamięć dzieloną '%s': deskryptor=%d, rozmiar=%zu B\n",
           name, fd, size);
    if (out_fd) *out_fd = fd;
    return addr;
}
 
void *shm_open_existing(const char *name, size_t size, int *out_fd)
{
    int fd = shm_open(name, O_RDWR, 0600);
    if (fd == -1) {
        perror("shm_open (open)");
        exit(EXIT_FAILURE);
    }
    void *addr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (addr == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }
    printf("[SHM] Otwarto pamięć dzieloną '%s': deskryptor=%d\n", name, fd);
    if (out_fd) *out_fd = fd;
    return addr;
}
 
void shm_remove(const char *name, void *addr, size_t size, int fd)
{
    if (munmap(addr, size) == -1) {
        perror("munmap");
        exit(EXIT_FAILURE);
    }
    if (close(fd) == -1) {
        perror("close (shm fd)");
        exit(EXIT_FAILURE);
    }
    if (shm_unlink(name) == -1) {
        perror("shm_unlink");
        exit(EXIT_FAILURE);
    }
    printf("[SHM] Usunięto pamięć dzieloną '%s'\n", name);
}