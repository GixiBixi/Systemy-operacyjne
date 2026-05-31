#ifndef BUFFER_H
#define BUFFER_H
#define BUF_COUNT     4
#define BUF_ELEM_SIZE 16
#define BUF_TOTAL     (BUF_COUNT * BUF_ELEM_SIZE)
 
typedef struct {
    int  in;
    int  out;
    char data[BUF_COUNT][BUF_ELEM_SIZE];
} SharedBuf;

#define SHM_SIZE  sizeof(SharedBuf)

#define DEFAULT_SEM_EMPTY  "/sem_empty_zad9"
#define DEFAULT_SEM_FULL   "/sem_full_zad9"
#define DEFAULT_SEM_MUTEX  "/sem_mutex_zad9"
#define DEFAULT_SHM_NAME   "/shm_zad9"
#define DEFAULT_PROD_FILE  "producer_data.txt"
#define DEFAULT_CONS_FILE  "consumer_data.txt"
 
#endif /* BUFFER_H */