#define _GNU_SOURCE
#define _POSIX_C_SOURCE 200112L
 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "cwiczenie3b.h"

void ex3b_parse_args_b1(int argc, char *argv[], int *out_sig, int *out_opt)
{
    if (argc != 3) {
        fprintf(stderr, "Uzycie: %s <numer_sygnalu> <opcja>\n", argv[0]);
        fprintf(stderr, "  opcja: 0 = domyslna, 1 = ignoruj, "
                        "2 = wlasna obsluga\n");
        fprintf(stderr, "Przyklad: %s 15 0\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    *out_sig = atoi(argv[1]);
    *out_opt = atoi(argv[2]);
}

void ex3b_parse_args_b2(int argc, char *argv[], int *out_sig, int *out_opt)
{
    if (argc != 3) {
        fprintf(stderr, "Uzycie: %s <numer_sygnalu> <opcja>\n", argv[0]);
        fprintf(stderr, "  opcja: 0 = domyslna, 1 = ignoruj, "
                        "2 = wlasna obsluga\n");
        fprintf(stderr, "Przyklad: %s 15 2\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    *out_sig = atoi(argv[1]);
    *out_opt = atoi(argv[2]);
}

pid_t ex3b_fork_and_exec(const char *child_prog,
                          const char *sig_str, const char *opt_str)
{
    pid_t pid = fork();
 
    if (pid < 0) {
        perror("fork");
        exit(EXIT_FAILURE);
    }
 
    if (pid == 0) {
        execlp(child_prog, child_prog, sig_str, opt_str, (char *)NULL);
        perror("execlp");
        _exit(EXIT_FAILURE);
    }
 
    return pid;
}

void ex3b_send_kill(pid_t pid, int sig)
{
    printf("[MACIERZYSTY PID=%d] Wysylam sygnal %d do PID=%d\n",
           getpid(), sig, pid);
    fflush(stdout);
 
    if (kill(pid, sig) == -1) {
        perror("kill");
        exit(EXIT_FAILURE);
    }
}

void ex3b_wait_and_report(pid_t pid)
{
    int status;
    pid_t ret;
 
    ret = waitpid(pid, &status, 0);
    if (ret == -1) {
        perror("waitpid");
        exit(EXIT_FAILURE);
    }
 
    printf("[MACIERZYSTY] Potomek PID=%d zakonczyl.\n", ret);
 
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