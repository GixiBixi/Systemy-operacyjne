#ifndef PAMIEC_H
#define PAMIEC_H
#include <stddef.h>
#include "common.h"
SegmentPD *shm_utworz(const char *nazwa, size_t rozmiar, int *des_out);
SegmentPD *shm_otworz(const char *nazwa, size_t rozmiar, int prot,
                      int *des_out);
void shm_odwzoruj_usun(SegmentPD *ptr, size_t rozmiar);
void shm_zamknij(int des, const char *nazwa);
void shm_usun(const char *nazwa);

#endif