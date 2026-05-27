Gabriela Stanek
Systemy operacyjne - temat 6

-------------------- Pamięć dzielona standardu POSIX --------------------

ZAWARTOŚĆ KATALOGU:
common.h – wspólne definicje typów, makr i stałych
semafory.h/.c – moduł operacji na semaforach nazwanych POSIX
pamiec.h/.c – moduł operacji na pamięci dzielonej POSIX
main.c – proces macierzysty (tworzy zasoby, fork+execlp, sprzątanie)
producent.c – producent (czyta z pliku, wstawia do bufora cyklicznego)
konsument.c – konsument (pobiera z bufora, zapisuje do pliku)
producent.txt  – przykładowy plik wejściowy dla producenta
Makefile – kompilacja, linkowanie, uruchamianie, diff, clean
lib/ – katalog na biblioteki (.a, .so)
obj/– katalog na pliki obiektowe (.o)
README – ten plik


DO CZEGO SŁUŻY PROGRAM?
Program demonstruje klasyczny problem producenta–konsumenta:

  • Producent czyta dane z pliku tekstowego porcjami po (NELE-1) bajtów
    i wstawia je do N-elementowego bufora cyklicznego w pamięci dzielonej.
  • Konsument pobiera porcje z bufora i zapisuje je do innego pliku.
  • Synchronizacja odbywa się przy użyciu dwóch semaforów POSIX:
      - PROD  (wolne miejsca, wartość początkowa = NBUF)
      - KONS  (gotowe porcje, wartość początkowa = 0)
  • Koniec danych sygnalizuje porcja z bajtem '\0' na pozycji 0.
  • Po zakończeniu oba pliki tekstowe są porównywane poleceniem diff.

Parametry bufora (plik common.h):
  NELE = 8   – rozmiar elementu bufora w bajtach (7 bajtów)
  NBUF = 5   – liczba elementów bufora

KOMPILACJA I URUCHOMIENIE:
  make - kompilacja (biblioteki + programy ze statyczną)
  make run - uruchomienie ze statyczną biblioteką + diff
  make run_dyn - uruchomienie z dynamiczną (LD_LIBRARY_PATH)
  make run_dyn2 - uruchomienie z dynamiczną (rpath wbudowany w ELF)
  make clean - usunięcie plików pośrednich