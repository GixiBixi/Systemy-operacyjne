#include "pipe_utils.h"     // dołącz nasz plik nagłówkowy
#include <stdio.h>          // dla perror()
#include <stdlib.h>         // dla exit()

void die(const char *msg)   // pomocnicza funkcja "zgiń z błędem"
{
    perror(msg);            // wypisz na stderr: msg + opis błędu systemowego (errno)
    exit(1);                // zakończ cały program z kodem błędu 1
}