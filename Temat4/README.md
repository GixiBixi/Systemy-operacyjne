===========================================================
Temat 4: Potoki
Gabriela Stanek
===========================================================

ZADANIE:
-------------
Problem "Producenta i Konsumenta"

Producent czyta dane z pliku tekstowego porcjami (po PRODUCER_CHUNK bajtow) i wysyla je przez potok. Konsument odbiera dane z potoku
porcjami (po CONSUMER_CHUNK bajtow) i zapisuje do innego pliku. Rozne rozmiary porcji i losowe opoznienia (sleep) symuluja rozne
predkosci dzialania obu procesow.

Po zakonczeniu plik wyjsciowy powinien byc identyczny z wejsciowym (mozna sprawdzic poleceniem: diff -s plik_wejsciowy plik_wyjsciowy).


PLIKI:
-----
pipe_utils.h - naglowek wspolnych stalych i funkcji pomocniczych
pipe_utils.c - implementacja funkcji pomocniczych
ex4_main.c - Cwiczenie 4: potok nienazwany, fork
ex5_producer.c - Cwiczenie 5: Producent dla potoku FIFO
ex5_consumer.c - Cwiczenie 5: Konsument dla potoku FIFO
ex5a_main.c - Cwiczenie 5(a): FIFO tworzony w programie, fork
Makefile - kompilacja i uruchamianie
README


KOMPILACJA:
----------
make


URUCHOMIENIE:
------------
Cwiczenie 4:
make run4

Cwiczenie 5(a):
make run5a

Cwiczenie 5(b):
make run5b


CZYSZCZENIE:
-----------
make clean
