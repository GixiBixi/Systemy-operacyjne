/*
Wywolac funkcj! fork trzy razy (najlepiej w p!tli for) i wypisac powyzsze identyfikatory dla procesu macierzystego oraz wszystkich procesów potomnych (najlepiej
dla ka’dego procesu w pojedynczej linii). Przy pomocy funkcji wait sprawi%, aby
proces macierzysty zaczeka& na zako#czenie wszystkich procesów potomnych.
Uwaga: Nie u’ywa% funkcji exit ani instrukcji return w procesie potomnym.
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

void print_ids(const char *label)
{
    printf("[%s] UID=%d GID=%d PID=%d PPID=%d PGID=%d\n",
           label,
           (int)getuid(), (int)getgid(),
           (int)getpid(), (int)getppid(), (int)getpgrp()); //wypisywanie identyfikatorów procesów
    fflush(stdout); //wymusza natychmiastowe wypisanie tekstu na ekran, bez tego rózne procesy moglyby mieszać swojewyniki bo kazdy ma inny bufor
}

int main(void)
{
    int i;
    pid_t pid; //przwchowuje wynik fork

    print_ids("RODZIC (przed fork)"); //wypisuje wyniki żeby mieć punkt odniesienia

    for (i = 0; i < 3; i++) { //petla tworzy trzy potomki,
        //tylko rodzic wykonuje wszystkie 3 iteracje - kazdy potomek trafia odrazu do case 0 i wychodzi z petli przez exit
        pid = fork();
        switch (pid) {
            case -1:
                perror("fork error"); //blad konczy program
                exit(EXIT_FAILURE);
            case 0:
                {
                    char label[32];
                    snprintf(label, sizeof(label), "POTOMEK %d", i + 1); //snprintf sklada tekst z kilku czesci w jeden napis i zapisuje go do zmiennej, 
                    // dziala tak samo jak printf - tylko zamiast wypisywać na ekran zapisuje wynik w zmiennej label
                    print_ids(label);
                    _exit(0); //potomek nigdy nie wraca do petli
                }
            default:
                break; // jestesmy w rodzicu (fork() zwrocilo PID potomka), nie robimy nic - break wraca do petli for i tworzy kolejnego potomka
        }
    }

    int status;
    while (wait(&status) > 0)
        ;
//rodzic czeka az wszytskie potomki sie skonczą

    print_ids("RODZIC (po wait)");
    return 0;
}
