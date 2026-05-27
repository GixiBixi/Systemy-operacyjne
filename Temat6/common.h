#ifndef COMMON_H
#define COMMON_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#define NELE  8
#define NBUF  5
#define DEFAULT_SHM_NAME   "/pd_shm"
#define DEFAULT_SEM_PROD   "/pd_sem_prod"
#define DEFAULT_SEM_KONS   "/pd_sem_kons"
#define DEFAULT_FILE_PROD  "producent.txt"
#define DEFAULT_FILE_KONS  "konsument.txt"

#define CheckError(ret, msg) \
    do { \
        if ((ret) == -1) { \
            perror(msg); \
            exit(EXIT_FAILURE); \
        } \
    } while (0)

#define CheckNull(ptr, msg) \
    do { \
        if ((ptr) == NULL) { \
            perror(msg); \
            exit(EXIT_FAILURE); \
        } \
    } while (0)

typedef struct {
    char element[NELE];
} Towar;

typedef struct {
    Towar bufor[NBUF];
    int   wstaw;
    int   wyjmij;
} SegmentPD;

#endif /* COMMON_H */