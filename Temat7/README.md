
Gabriela Stanek

================================================================
  KOLEJKI KOMUNIKATOW POSIX
  Cwiczenie 8
================================================================

ZAWARTOSC KATALOGU:

  common.h        – wspolny plik naglowkowy: nazwa kolejki serwera, stale MSG_MAX_SIZE i QUEUE_MAX_MSGS
  mqueue_lib.h    – interfejs biblioteki opakowujacej kolejki POSIX: MQ_Create, MQ_Open, MQ_Close, MQ_Unlink,
                    MQ_Send, MQ_Receive, MQ_PrintAttr, makro CheckError
  mqueue_lib.c    – implementacja biblioteki; kazda funkcja wywoluje perror() przy bledzie i zwraca 0 przy porazce
  serwer.c        – program serwera (demon arytmetyczny)
  klient.c        – program klienta (interaktywny kalkulator)
  Makefile        – kompilacja, budowa bibliotek, uruchamianie
  README          – ten plik

DO CZEGO SLUZY PROGRAM?

Program demonstruje komunikacje miedzy-procesowa (IPC) przy uzyciu
kolejek komunikatow standardu POSIX.

Zaimplementowany jest schemat klient-serwer:

  KLIENT wysyla do serwera zadanie postaci:
      "<PID_klienta> <wyrazenie>"
  np. "17895 2+3" lub "17895 10.5/2"

  SERWER oblicza wynik i odsyla go do kolejki klienta:
      "5" lub "5.25"

Obslugiwane operatory: +  -  *  /
Obsluga dzielenia przez zero: serwer zwraca komunikat bledu.

Wielu klientow moze dzialac jednoczesnie – kazdy ma wlasna
kolejke odpowiedzi o nazwie /<PID>.


SPOSOB ROZWIAZANIA:

1. SERWER (serwer.c):
   - Tworzy kolejke /mq_serwer
   - Wypisuje atrybuty kolejki
   - Dziala w petli nieskonczonej

2. KLIENT (klient.c):
   - Tworzy wlasna kolejke /<PID>
   - Wypisuje atrybuty kolejki
   - Otwiera kolejke serwera
   - EOF (Ctrl+D) lub SIGINT

3. BIBLIOTEKA (mqueue_lib):
   - Opakowuje mq_open/close/send/receive/unlink/getattr
   - Jednolita diagnostyka bledow przez perror()
   - Kazda funkcja zwraca 0 przy porazce -> dziala z makrem CheckError
   - Budowana jako libmqueue.a (statyczna) i libmqueue.so (dzielona)


KOMPILACJA

  make

Tworzy:
  libmqueue.a      – biblioteka statyczna
  libmqueue.so     – biblioteka dzielona
  serwer.x         – serwer (linkowany statycznie z libmqueue.a)
  klient.x         – klient (linkowany statycznie z libmqueue.a)

URUCHOMIENIE:
biblioteka statyczna - make run_static
biblioteka dzielona (LD_LIBRARY_PATH) -  make run_so_1
biblioteka dzielona (rpath wbudowany w ELF) - make run_so_2

CZYSZCZENIE:
make clean