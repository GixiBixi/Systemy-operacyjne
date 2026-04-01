#define _GNU_SOURCE
#define _POSIX_C_SOURCE 200112L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include "cwiczenie3a.h"

void ex3a_parse_args(int argc, char *argv[], int *out_sig, int *out_opt)
{
    if (argc != 3) {
        fprintf(stderr, "Uzycie: %s <numer_sygnalu> <opcja>\n", argv[0]);
        fprintf(stderr, "  opcja: %d = domyslna akcja\n", OPT_DEFAULT);
        fprintf(stderr, "         %d = ignoruj sygnal\n", OPT_IGNORE);
        fprintf(stderr, "         %d = wlasna obsluga\n", OPT_CUSTOM);
        fprintf(stderr, "Przyklad: %s 15 2\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    *out_sig = atoi(argv[1]);
    *out_opt = atoi(argv[2]);
}

void ex3a_sighandler(int sig)
{
    printf("  [PID %d] Otrzymano sygnal %d (%s)\n",
           getpid(), sig, strsignal(sig));
    fflush(stdout);
}

void ex3a_run(int sig, int opt)
{
    void (*handler)(int);
 
    switch (opt) {
        case OPT_DEFAULT:
            handler = SIG_DFL;
            printf("[PID %d] Obsluga sygnalu %d: DOMYSLNA\n",
                   getpid(), sig);
            break;
        case OPT_IGNORE:
            handler = SIG_IGN;
            printf("[PID %d] Obsluga sygnalu %d: IGNOROWANIE\n",
                   getpid(), sig);
            break;
        case OPT_CUSTOM:
            handler = ex3a_sighandler;
            printf("[PID %d] Obsluga sygnalu %d: WLASNA\n",
                   getpid(), sig);
            break;
        default:
            fprintf(stderr, "Nieznana opcja: %d\n", opt);
            exit(EXIT_FAILURE);
    }
 
    if (signal(sig, handler) == SIG_ERR) {
        perror("signal");
        exit(EXIT_FAILURE);
    }
 
    printf("[PID %d] Czekam na sygnal...\n", getpid());
    fflush(stdout);
 
    while (1) {
        sleep(1);
        pause();
    }
}