#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>

#include "common.h"
#include "mqueue_lib.h"

static mqd_t g_client_mqd = (mqd_t)(-1);
static mqd_t g_server_mqd = (mqd_t)(-1);
static char  g_client_queue_name[64];

/* ------------------------------------------------------------------ */
static void cleanup(void)
{
    if (g_client_mqd != (mqd_t)(-1)) {
        printf("[KLIENT %d] Zamykam i usuwam kolejke '%s'.\n",
               (int)getpid(), g_client_queue_name);
        MQ_Close(g_client_mqd);
        g_client_mqd = (mqd_t)(-1);
        MQ_Unlink(g_client_queue_name);
    }
    if (g_server_mqd != (mqd_t)(-1)) {
        MQ_Close(g_server_mqd);
        g_server_mqd = (mqd_t)(-1);
    }
}

static void sigint_handler(int sig)
{
    (void)sig;
    printf("\n[KLIENT %d] Odebrano SIGINT - koncze prace.\n", (int)getpid());
    exit(EXIT_SUCCESS);
}

/* ------------------------------------------------------------------ */
int main(void)
{
    pid_t my_pid = getpid();

    printf("=================================================\n");
    printf(" KLIENT obliczen arytmetycznych\n");
    printf(" Autor: Gabriela Stanek | Kolejki komunikatow POSIX\n");
    printf(" PID klienta: %d\n", (int)my_pid);
    printf("=================================================\n\n");

    if (atexit(cleanup) != 0) {
        perror("atexit");
        exit(EXIT_FAILURE);
    }

    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = sigint_handler;
    sigemptyset(&sa.sa_mask);
    if (sigaction(SIGINT, &sa, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }

    snprintf(g_client_queue_name, sizeof(g_client_queue_name), "/%d", (int)my_pid);

    mq_unlink(g_client_queue_name);

    g_client_mqd = MQ_Create(g_client_queue_name, QUEUE_MAX_MSGS, MSG_MAX_SIZE, 0660);
    if (g_client_mqd == (mqd_t)0) {
        fprintf(stderr, "[KLIENT %d] Nie mozna utworzyc kolejki '%s'.\n",
                (int)my_pid, g_client_queue_name);
        exit(EXIT_FAILURE);
    }
    printf("[KLIENT %d] Kolejka '%s' utworzona (deskryptor: %d).\n",
           (int)my_pid, g_client_queue_name, (int)g_client_mqd);
    MQ_PrintAttr(g_client_mqd, "KLIENT");

    g_server_mqd = MQ_Open(SERVER_QUEUE_NAME, O_WRONLY);
    if (g_server_mqd == (mqd_t)0) {
        fprintf(stderr, "[KLIENT %d] Nie mozna otworzyc kolejki serwera '%s'.\n",
                (int)my_pid, SERVER_QUEUE_NAME);
        exit(EXIT_FAILURE);
    }
    printf("[KLIENT %d] Otwarto kolejke serwera '%s' (deskryptor: %d).\n\n",
           (int)my_pid, SERVER_QUEUE_NAME, (int)g_server_mqd);

    char reply[MSG_MAX_SIZE + 1];
    char line[MSG_MAX_SIZE - 12];
    char msg[MSG_MAX_SIZE];

    printf("[KLIENT %d] Wpisz wyrazenie (np. 2+3, 10/4, 7*8) lub Ctrl+D aby zakonczyc.\n\n",
           (int)my_pid);

    while (fgets(line, sizeof(line), stdin) != NULL) {
        size_t len = strlen(line);
        if (len > 0 && line[len - 1] == '\n')
            line[--len] = '\0';
        if (len == 0)
            continue;

        snprintf(msg, sizeof(msg), "%d %s", (int)my_pid, line);
        printf("[KLIENT %d] Wysylam: '%s'\n", (int)my_pid, msg);

        CheckError(MQ_Send(g_server_mqd, msg, strlen(msg) + 1, 0),
                   "wyslanie zadania do serwera");

        unsigned int prio = 0;
        ssize_t n = MQ_Receive(g_client_mqd, reply, MSG_MAX_SIZE, &prio);
        if (n <= 0) {
            fprintf(stderr, "[KLIENT %d] Blad odbioru odpowiedzi.\n", (int)my_pid);
            continue;
        }
        reply[n] = '\0';
        printf("[KLIENT %d] Wynik: %s\n\n", (int)my_pid, reply);
    }

    printf("[KLIENT %d] EOF - koncze prace.\n", (int)my_pid);
    return 0;
}