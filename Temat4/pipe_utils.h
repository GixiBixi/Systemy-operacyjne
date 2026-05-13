#ifndef PIPE_UTILS_H // jeśli ten plik nie był jeszcze dołączony...
#define PIPE_UTILS_H  // ...zaznacz że teraz jest (ochrona przed podwójnym include)

#include <stdlib.h> // potrzebne dla exit()
#include <stdio.h> // potrzebne dla perror()

#define PRODUCER_CHUNK 7 // producent czyta/wysyła porcje po 7 bajtów
#define CONSUMER_CHUNK 11 // konsument odbiera porcje po 11 bajtów
#define MAX_SLEEP 2 // maksymalny czas uśpienia (sekundy)

void die(const char *msg); // deklaracja funkcji die (definicja w .c)

#endif // koniec ochrony przed podwójnym include