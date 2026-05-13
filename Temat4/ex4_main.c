#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <time.h>
#include "pipe_utils.h"

static void producent(int pipe_write_fd, const char *src_path)
// static = widoczna tylko w tym pliku
// pipe_write_fd = deskryptor końca potoku do PISANIA
// src_path = ścieżka pliku źródłowego
{
    int src_fd; // deskryptor pliku źródłowego
    ssize_t n; // ile bajtów faktycznie odczytano
    char buf[PRODUCER_CHUNK]; // bufor = 7 bajtów
 
    src_fd = open(src_path, O_RDONLY); // otwórz plik źródłowy tylko do odczytu
    if (src_fd < 0)
        die("Producent: open (plik zrodlowy)"); // błąd = wypisz i zakończ
 
    printf("[Producent] Uruchomiony. Czytam z '%s', porcje po %d B.\n",
           src_path, PRODUCER_CHUNK); // komunikat startowy
 
    while ((n = read(src_fd, buf, sizeof(buf))) > 0) {
        // czytaj porcjami po 7 bajtów; n = ile faktycznie wczytano; pętla kończy się gdy n==0 (EOF)
        write(STDOUT_FILENO, "[Producent] Wysylam: ", 21); // wypisz prefix na ekran
        write(STDOUT_FILENO, buf, n); // wypisz porcję danych na ekran
        write(STDOUT_FILENO, "\n", 1); // nowa linia
 
        if (write(pipe_write_fd, buf, n) != n) // wyślij tę samą porcję do potoku
            die("Producent: write (potok)"); // błąd jeśli nie zapisano wszystkich bajtów
 
        sleep(rand() % (MAX_SLEEP + 1)); // uśpij losowo 0-2 sekundy (symulacja różnych prędkości)
    }
 
    if (n < 0)
        die("Producent: read (plik zrodlowy)"); // n<0 oznacza błąd odczytu (nie EOF)
 
    close(src_fd); // zamknij plik źródłowy
    close(pipe_write_fd); // zamknij koniec zapisu potoku → konsument dostanie EOF
    printf("[Producent] Zakonczono. Wszystkie dane wyslane.\n");
}
 
static void konsument(int pipe_read_fd, const char *dst_path)
// pipe_read_fd = deskryptor końca potoku do CZYTANIA
// dst_path = ścieżka pliku docelowego
{
    int dst_fd;
    ssize_t n;
    char buf[CONSUMER_CHUNK]; // bufor = 11 bajtów (celowo inny niż u producenta!)
 
    dst_fd = open(dst_path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    // otwórz plik docelowy: tylko do zapisu | stwórz jeśli nie istnieje | obetnij do 0 jeśli istnieje
    // prawa dostępu: rw-r--r-- (0644)
    if (dst_fd < 0)
        die("Konsument: open (plik docelowy)");
 
    printf("[Konsument] Uruchomiony. Zapisuje do '%s', porcje po %d B.\n",
           dst_path, CONSUMER_CHUNK);
 
    while ((n = read(pipe_read_fd, buf, sizeof(buf))) > 0) {
    // czytaj z potoku porcjami po 11 bajtów; pętla kończy się gdy producent zamknie swój koniec (EOF)
        write(STDOUT_FILENO, "[Konsument] Odebrano: ", 22); // wypisz prefix na ekran
        write(STDOUT_FILENO, buf, n); // wypisz odebraną porcję
        write(STDOUT_FILENO, "\n", 1);

        if (write(dst_fd, buf, n) != n) // zapisz odebraną porcję do pliku docelowego
            die("Konsument: write (plik docelowy)");

        sleep(rand() % (MAX_SLEEP + 1)); // losowe uśpienie 0-2 sek
    }
 
    if (n < 0)
        die("Konsument: read (potok)");
 
    close(dst_fd); // zamknij plik docelowy
    close(pipe_read_fd); // zamknij koniec odczytu potoku
    printf("[Konsument] Zakonczono. Wszystkie dane zapisane.\n");
}
 
int main(int argc, char *argv[])
{
    int pipefd[2];  // tablica na dwa deskryptory potoku: [0]=odczyt, [1]=zapis
    pid_t pid;      // PID procesu potomnego

    if (argc != 3) {  // program wymaga dokładnie 2 argumentów (plik_źródłowy, plik_docelowy)
        fprintf(stderr, "Uzycie: %s plik_zrodlowy plik_docelowy\n", argv[0]);
        exit(1);
    }

    srand((unsigned)time(NULL)); // inicjalizuj generator losowy aktualnym czasem

    if (pipe(pipefd) < 0)  // utwórz potok nienazwany; pipefd[0]=czytanie, pipefd[1]=pisanie
        die("pipe");

    printf("=== Cwiczenie 4 ===\n");

    pid = fork(); // utwórz proces potomny; w potomnym pid==0, w ojcu pid==PID_dziecka
    if (pid < 0)
        die("fork");

    if (pid == 0) {          // === PROCES POTOMNY (syn) = PRODUCENT ===
        close(pipefd[0]);    // zamknij koniec odczytu – producent nie będzie czytał
        producent(pipefd[1], argv[1]); // produkuj dane z argv[1] do potoku
        exit(0);             // zakończ proces potomny

    } else {                 // === PROCES MACIERZYSTY (ojciec) = KONSUMENT ===
        close(pipefd[1]);    // zamknij koniec zapisu – konsument nie będzie pisał
        konsument(pipefd[0], argv[2]); // odbieraj z potoku i zapisuj do argv[2]
        wait(NULL);          // czekaj aż syn (producent) się zakończy
    }

    printf("\nOba procesy zakonczone.\n");
    return 0;
}