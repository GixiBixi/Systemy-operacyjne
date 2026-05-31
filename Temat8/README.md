
Gabriela Sanek

DO CZEGO SŁUŻY PROGRAM:
-----------------------
Program demonstruje klasyczny problem producenta i konsumenta z buforem
cyklicznym (okrężnym). Dane przesyłane są między dwoma oddzielnymi procesami
za pomocą pamięci dzielonej POSIX. Synchronizacja opiera się na trzech
semaforach nazwanych POSIX:
  - sem_empty  – liczba wolnych slotów w buforze (init = BUF_COUNT)
  - sem_full   – liczba zajętych slotów (init = 0)
  - sem_mutex  – wzajemne wykluczanie przy dostępie do indeksów (init = 1)

Bufor ma BUF_COUNT=4 elementów, każdy o rozmiarze BUF_ELEM_SIZE=16 B.

ZAWARTOŚĆ KATALOGU:
-------------------
  main.c – proces macierzysty: tworzy IPC, fork+execlp, sprzątanie
  producer.c – producent: czyta plik, wstawia porcje do bufora
  consumer.c – konsument: pobiera porcje z bufora, zapisuje do pliku
  buffer.h – definicja struktury bufora i stałych
  sem_utils.h/c – moduł operacji na semaforach POSIX
  shm_utils.h/c – moduł operacji na pamięci dzielonej POSIX
  Makefile – kompilacja, linkowanie i uruchamianie programu
  README – ten plik

SPOSÓB ROZWIĄZANIA:
-------------------
Proces macierzysty (main) tworzy semafory i pamięć dzieloną, a następnie
za pomocą fork()+execlp() uruchamia producenta i konsumenta jako oddzielne
procesy potomne. Nazwy semaforów, pamięci dzielonej i plików tekstowych
przekazywane są do potomków przez argumenty execlp.

Producent czyta dane z pliku wejściowego porcjami po (BUF_ELEM_SIZE-1) bajtów
i wstawia je do bufora. Na koniec wysyła element z bajtem '\0' jako marker EOF.

Konsument pobiera elementy z bufora i zapisuje je do pliku wyjściowego.
Po odebraniu markera EOF kończy pracę.

Dane przesyłane są jako surowe bajty (read/write), nie jako sformatowany tekst.

Po zakończeniu obu potomków proces macierzysty usuwa semafory i pamięć dzieloną,
a Makefile porównuje pliki producenta i konsumenta poleceniem diff.

JAK URUCHOMIĆ:
--------------
  make run_static – z biblioteką statyczną (libutils.a)
  make run_shared – z biblioteką dzieloną (LD_LIBRARY_PATH=.)
  make run_shared_rpath – z biblioteką dzieloną (rpath wbudowany)
  make clean – usuwa pliki wynikowe