#include "pamiec.h"
#include "common.h"

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

SegmentPD *shm_utworz(const char *nazwa, size_t rozmiar, int *des_out)
{
    int des = shm_open(nazwa, O_CREAT | O_EXCL | O_RDWR,
                       S_IRUSR | S_IWUSR);
    if (des == -1) {
        perror("shm_open (tworzenie)");
        fprintf(stderr, "  nazwa pamieci dzielonej: %s\n", nazwa);
        exit(EXIT_FAILURE);
    }

    if (ftruncate(des, (off_t)rozmiar) == -1) {
        perror("ftruncate");
        exit(EXIT_FAILURE);
    }

    void *ptr = mmap(NULL, rozmiar, PROT_READ | PROT_WRITE,
                     MAP_SHARED, des, 0);
    if (ptr == MAP_FAILED) {
        perror("mmap (tworzenie)");
        exit(EXIT_FAILURE);
    }

    if (des_out) *des_out = des;
    return (SegmentPD *)ptr;
}

SegmentPD *shm_otworz(const char *nazwa, size_t rozmiar, int prot,
                      int *des_out)
{
    int oflag = (prot & PROT_WRITE) ? O_RDWR : O_RDONLY;

    int des = shm_open(nazwa, oflag, 0);
    if (des == -1) {
        perror("shm_open (otwieranie)");
        fprintf(stderr, "  nazwa pamieci dzielonej: %s\n", nazwa);
        exit(EXIT_FAILURE);
    }

    void *ptr = mmap(NULL, rozmiar, prot, MAP_SHARED, des, 0);
    if (ptr == MAP_FAILED) {
        perror("mmap (otwieranie)");
        exit(EXIT_FAILURE);
    }

    if (des_out) *des_out = des;
    return (SegmentPD *)ptr;
}

void shm_odwzoruj_usun(SegmentPD *ptr, size_t rozmiar)
{
    if (munmap(ptr, rozmiar) == -1) {
        perror("munmap");
    }
}

void shm_zamknij(int des, const char *nazwa)
{
    (void)nazwa;
    if (close(des) == -1) {
        perror("close (shm)");
    }
}

void shm_usun(const char *nazwa)
{
    if (shm_unlink(nazwa) == -1) {
        perror("shm_unlink");
        fprintf(stderr, "  nazwa pamieci dzielonej: %s\n", nazwa);
    }
}