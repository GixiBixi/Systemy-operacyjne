                    Ćwiczenie 6
-> Gabriela Stanek

ZAWARTOŚĆ
--------------------------------------------------------

  powielacz.c     – główny program; tworzy semafor, plik numer.txt,
                    forkuje procesy potomne (execlp), czeka na ich
                    koniec i sprawdza wynik synchronizacji

  wykluczanie.c   – proces potomny; wielokrotnie wchodzi do sekcji
                    krytycznej (chronionej semaforem) i modyfikuje
                    licznik w pliku numer.txt

  semafor.h/.c    – własna biblioteka opakowująca funkcje POSIX:
                    sem_utworz, sem_otworz, sem_wartosc,
                    sem_opusc, sem_podniesc, sem_zamknij, sem_usun

  checkerror.h    – makro CheckError(Arg): jeśli Arg==0/NULL,
                    wypisuje lokalizację błędu i wywołuje exit()

  Makefile        – kompilacja, linkowanie, przykładowe uruchomienie
  README          – ten plik



OPIS ROZWIĄZANIA
--------------------------------------------------------

Problem: N procesów chce wielokrotnie modyfikować wspólny zasób
(licznik w pliku numer.txt). Bez synchronizacji może dojść do
wyścigu danych (race condition) – kilka procesów odczyta tę samą
wartość i ją nadpisze, gubią się inkrementacje.

Rozwiązanie: semafor binarny (wartość początkowa 1) chroni
sekcję krytyczną wzorcem:
    P(S) → sekcja krytyczna → V(S)

Semafor jest nazwany (/wykluczanie_sem), więc jest widoczny
we wszystkich procesach potomnych (mimo exec).

Porządki po zakończeniu:
  - powielacz rejestruje cleanup() przez atexit() → sem_unlink
  - obsługa SIGINT wywołuje exit() → uruchamia cleanup()
  - procesy potomne używają _exit() po exec (nie wywołują atexit)

Weryfikacja poprawności:
  Jeśli synchronizacja działa poprawnie, końcowy numer w pliku
  powinien wynosić dokładnie: liczba_procesow × liczba_sekcji.


KOMPILACJA I URUCHOMIENIE:
--------------------------------------------------------

  make              # kompiluje oba programy

  make run          # uruchamia z domyślnymi parametrami (3 proc., 2 sekcje)

  ./powielacz <N> <K>
                    # N = liczba procesów, K = liczba sekcji krytycznych

Przykład:
  ./powielacz 4 3   # 4 procesy, każdy wchodzi 3 razy → oczekiwany wynik: 12
