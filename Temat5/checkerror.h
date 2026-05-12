#ifndef CHECKERROR_H
#define CHECKERROR_H
 
#include <stdio.h>
#include <stdlib.h>
 
#define CheckError(Arg) \
    do { \
        if (!(Arg)) { \
            fprintf(stderr, "[ERROR] %s:%d (%s) -> CheckError argument: %s\n", \
                    __FILE__, __LINE__, __func__, #Arg); \
            exit(EXIT_FAILURE); \
        } \
    } while (0)
 
#endif /* CHECKERROR_H */