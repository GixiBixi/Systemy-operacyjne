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

static mqd_t g_server_mqd = (mqd_t)(-1);

/* ------------------------------------------------------------------ */
static void cleanup(void)
{
    if (g_server_mqd != (mqd_t)(-1)) {
        printf("[SERWER] Zamykam i usuwam kolejke '%s'.\n", SERVER_QUEUE_NAME);
        MQ_Close(g_server_mqd);
        g_server_mqd = (mqd_t)(-1);
    }
    MQ_Unlink(SERVER_QUEUE_NAME);
}

static void sigint_handler(int sig)
{
    (void)sig;
    printf("\n[SERWER] Odebrano SIGINT - koncze prace.\n");
    exit(EXIT_SUCCESS);
}

/* ------------------------------------------------------------------ */
static int calculate(const char *expr, double *result)
{
    double a, b;
    char op;
    if (sscanf(expr, "%lf%c%lf", &a, &op, &b) != 3)
        return 0;
    switch (op) {
        case '+': *result = a + b; break;
        case '-': *result = a - b; break;
        case '*': *result = a * b; break;
        case '/':
            if (b == 0.0) { *result = 0.0; return 0; }
            *result = a / b;
            break;
        default: return 0;
    }
    return 1;
}

/* ------------------------------------------------------------------ */
int main(void)
{
    printf("=================================================\n");
    printf(" SERWER obliczen arytmetycznych\n");
    printf(" Autor: Gabriela Stanek | Kolejki komunikatow POSIX\n");
    printf(" PID serwera: %d\n", (int)getpid());
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

    mq_unlink(SERVER_QUEUE_NAME);

    g_server_mqd = MQ_Create(SERVER_QUEUE_NAME, QUEUE_MAX_MSGS, MSG_MAX_SIZE, 0660);
    if (g_server_mqd == (mqd_t)0) {
        fprintf(stderr, "[SERWER] Nie mozna utworzyc kolejki.\n");
        exit(EXIT_FAILURE);
    }
    printf("[SERWER] Kolejka '%s' utworzona (deskryptor: %d).\n",
           SERVER_QUEUE_NAME, (int)g_server_mqd);
    MQ_PrintAttr(g_server_mqd, "SERWER");
    printf("\n[SERWER] Czekam na polaczenia klientow...\n\n");

    char buf[MSG_MAX_SIZE + 1];

    for (;;) {
        unsigned int prio = 0;
        ssize_t n = MQ_Receive(g_server_mqd, buf, MSG_MAX_SIZE, &prio);
        if (n <= 0)
            continue;
        buf[n] = '\0';

        printf("[SERWER] Odebrano komunikat (prio=%u, %zd B): '%s'\n", prio, n, buf);

        pid_t client_pid;
        char  expr[MSG_MAX_SIZE];
        if (sscanf(buf, "%d %s", (int*)&client_pid, expr) != 2) {
            printf("[SERWER] Bledny format komunikatu - ignoruje.\n");
            continue;
        }
        printf("[SERWER] Klient PID=%d, wyrazenie='%s'\n", (int)client_pid, expr);

        double result = 0.0;
        char   reply[MSG_MAX_SIZE];
        if (calculate(expr, &result))
            snprintf(reply, sizeof(reply), "%.6g", result);
        else
            snprintf(reply, sizeof(reply), "BLAD: '%.*s'",
                     (int)(sizeof(reply) - 10), expr);

        printf("[SERWER] Wynik: %s\n\n", reply);

        char client_queue_name[64];
        snprintf(client_queue_name, sizeof(client_queue_name), "/%d", (int)client_pid);

        mqd_t client_mqd = MQ_Open(client_queue_name, O_WRONLY);
        if (client_mqd == (mqd_t)0) {
            fprintf(stderr, "[SERWER] Nie mozna otworzyc kolejki klienta '%s'.\n",
                    client_queue_name);
            continue;
        }
        CheckError(MQ_Send(client_mqd, reply, strlen(reply) + 1, 0),
                   "wyslanie odpowiedzi do klienta");
        MQ_Close(client_mqd);
    }

    return 0;
}