#define _GNU_SOURCE
#define _POSIX_C_SOURCE 200112L
#define _XOPEN_SOURCE 500

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "cwiczenie3c.h"
 
void ex3c_parse_args_c1(int argc, char *argv[],
                         int *out_sig, int *out_opt, int *out_nchildren)
{
    if (argc < 3 || argc > 4) {
        fprintf(stderr, "Uzycie: %s <numer_sygnalu> <opcja> "
                        "[liczba_potomkow]\n", argv[0]);
        fprintf(stderr, "  opcja: 0 = domyslna, 1 = ignoruj, "
                        "2 = wlasna obsluga\n");
        fprintf(stderr, "  liczba_potomkow: domyslnie 3\n");
        fprintf(stderr, "Przyklad: %s 15 0 3\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    *out_sig       = atoi(argv[1]);
    *out_opt       = atoi(argv[2]);
    *out_nchildren = (argc == 4) ? atoi(argv[3]) : 3;
}

void ex3c_parse_args_c2(int argc, char *argv[],
                         int *out_sig, int *out_opt, int *out_nchildren)
{
    if (argc != 4) {
        fprintf(stderr, "Uzycie: %s <numer_sygnalu> <opcja> "
                        "<liczba_potomkow>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    *out_sig       = atoi(argv[1]);
    *out_opt       = atoi(argv[2]);
    *out_nchildren = atoi(argv[3]);
}

void ex3c_parse_args_c3(int argc, char *argv[], int *out_sig, int *out_opt)
{
    if (argc != 3) {
        fprintf(stderr, "Uzycie: %s <numer_sygnalu> <opcja>\n", argv[0]);
        fprintf(stderr, "  opcja: 0 = domyslna, 1 = ignoruj, "
                        "2 = wlasna obsluga\n");
        exit(EXIT_FAILURE);
    }
    *out_sig = atoi(argv[1]);
    *out_opt = atoi(argv[2]);
}

pid_t ex3c_fork_and_exec(const char *prog, char *const args[])
{
    pid_t pid = fork();
 
    if (pid < 0) {
        perror("fork");
        exit(EXIT_FAILURE);
    }
 
    if (pid == 0) {
        execvp(prog, args);
        perror("execvp");
        _exit(EXIT_FAILURE);
    }
 
    return pid;
}

void ex3c_become_leader(void)
{
    if (setpgid(0, 0) == -1) {
        perror("setpgid");
        exit(EXIT_FAILURE);
    }
    printf("[PID %d] Jestem liderem grupy PGID=%d\n",
           getpid(), getpgrp());
    fflush(stdout);
}

void ex3c_ignore_signal(int sig)
{
    if (signal(sig, SIG_IGN) == SIG_ERR) {
        perror("signal SIG_IGN");
        exit(EXIT_FAILURE);
    }
    printf("[PID %d] Ignoruje sygnal %d\n", getpid(), sig);
    fflush(stdout);
}

void ex3c_send_kill_group(pid_t pgid, int sig)
{
    printf("[PID %d] Wysylam sygnal %d do grupy PGID=%d\n",
           getpid(), sig, pgid);
    fflush(stdout);
 
    if (kill(-pgid, sig) == -1) {
        perror("kill (group)");
        exit(EXIT_FAILURE);
    }
}

void ex3c_wait_and_report(pid_t pid)
{
    int status;
    pid_t ret;
 
    ret = waitpid(pid, &status, 0);
    if (ret == -1) {
        perror("waitpid");
        exit(EXIT_FAILURE);
    }
 
    printf("[PID %d] Potomek PID=%d zakonczyl.\n", getpid(), ret);
 
    if (WIFEXITED(status)) {
        printf("  Kod wyjscia: %d\n", WEXITSTATUS(status));
    } else if (WIFSIGNALED(status)) {
        int sig_nr = WTERMSIG(status);
        printf("  Zakonczony przez sygnal nr %d (%s)\n",
               sig_nr, strsignal(sig_nr));
    } else if (WIFSTOPPED(status)) {
        printf("  Zatrzymany przez sygnal nr %d (%s)\n",
               WSTOPSIG(status), strsignal(WSTOPSIG(status)));
    }
 
    fflush(stdout);
}
void ex3c_wait_all(void)
{
    int status;
    pid_t ret;
 
    while ((ret = wait(&status)) > 0) {
        printf("[PID %d] Potomek PID=%d zakonczyl.\n", getpid(), ret);
 
        if (WIFEXITED(status)) {
            printf("  Kod wyjscia: %d\n", WEXITSTATUS(status));
        } else if (WIFSIGNALED(status)) {
            int sig_nr = WTERMSIG(status);
            printf("  Zakonczony przez sygnal nr %d (%s)\n",
                   sig_nr, strsignal(sig_nr));
        }
 
        fflush(stdout);
    }
}
