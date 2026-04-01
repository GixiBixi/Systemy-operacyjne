#define _GNU_SOURCE
#define _POSIX_C_SOURCE 200112L
#define _XOPEN_SOURCE 500
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include "cwiczenie3a.h"
#include "cwiczenie3b.h"
#include "cwiczenie3c.h"

static void prog_ex3a(int argc, char *argv[])
{
    int sig, opt;
    ex3a_parse_args(argc, argv, &sig, &opt);
    ex3a_run(sig, opt);
}


static void prog_ex3b1(int argc, char *argv[])
{
    int   sig, opt;
    pid_t child;
    ex3b_parse_args_b1(argc, argv, &sig, &opt);
    child = ex3b_fork_and_exec("./ex3b2", argv[1], argv[2]);
    printf("[MACIERZYSTY PID=%d] Uruchomiono potomka PID=%d\n",
           getpid(), child);
    fflush(stdout);
    sleep(2);
    ex3b_send_kill(child, sig);
    ex3b_wait_and_report(child);
}

static void prog_ex3b2(int argc, char *argv[])
{
    int sig, opt;
    ex3b_parse_args_b2(argc, argv, &sig, &opt);
    ex3a_run(sig, opt);
}


static void prog_ex3c1(int argc, char *argv[])
{
    int   sig, opt, nchildren;
    pid_t child;
    pid_t child_pgid;
    char  nch_str[16];
    char *args[] = { "./ex3c2", argv[1], argv[2], nch_str, NULL };

    ex3c_parse_args_c1(argc, argv, &sig, &opt, &nchildren);
    snprintf(nch_str, sizeof(nch_str), "%d", nchildren);

    child = ex3c_fork_and_exec("./ex3c2", args);
    printf("[C1 PID=%d] Uruchomiono c2 z PID=%d\n", getpid(), child);
    fflush(stdout);

    sleep(3);

    child_pgid = getpgid(child);
    if (child_pgid == -1) {
        perror("getpgid");
        exit(EXIT_FAILURE);
    }

    ex3c_send_kill_group(child_pgid, sig);

    ex3c_wait_and_report(child);
}

static void prog_ex3c2(int argc, char *argv[])
{
    int  sig, opt, nchildren, i;
    char *args[4];
    ex3c_parse_args_c2(argc, argv, &sig, &opt, &nchildren);
    ex3c_ignore_signal(sig);
    ex3c_become_leader();

    args[0] = "./ex3c3";
    args[1] = argv[1];
    args[2] = argv[2];
    args[3] = NULL;

    for (i = 0; i < nchildren; i++) {
        pid_t cpid = ex3c_fork_and_exec("./ex3c3", args);
        printf("[C2 PID=%d] Uruchomiono c3 nr %d, PID=%d\n",
               getpid(), i + 1, cpid);
        fflush(stdout);
    }


    ex3c_wait_all();

    printf("[C2 PID=%d] Wszyscy potomkowie zakonczyli.\n", getpid());
    fflush(stdout);
}

static void prog_ex3c3(int argc, char *argv[])
{
    int sig, opt;
    ex3c_parse_args_c3(argc, argv, &sig, &opt);
    ex3a_run(sig, opt);
}

int main(int argc, char *argv[])
{

    const char *prog = strrchr(argv[0], '/');
    prog = prog ? prog + 1 : argv[0];

    if      (strcmp(prog, "ex3a")  == 0) prog_ex3a(argc,  argv);
    else if (strcmp(prog, "ex3b1") == 0) prog_ex3b1(argc, argv);
    else if (strcmp(prog, "ex3b2") == 0) prog_ex3b2(argc, argv);
    else if (strcmp(prog, "ex3c1") == 0) prog_ex3c1(argc, argv);
    else if (strcmp(prog, "ex3c2") == 0) prog_ex3c2(argc, argv);
    else if (strcmp(prog, "ex3c3") == 0) prog_ex3c3(argc, argv);
    else {
        fprintf(stderr, "Nieznany program: %s\n"
                        "Uruchom jako: ex3a, ex3b1, ex3b2, "
                        "ex3c1, ex3c2 lub ex3c3\n", prog);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}