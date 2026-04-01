#ifndef EX3B_H
#define EX3B_H

#include <sys/types.h>

void ex3b_parse_args_b1(int argc, char *argv[], int *out_sig, int *out_opt);
void ex3b_parse_args_b2(int argc, char *argv[], int *out_sig, int *out_opt);
pid_t ex3b_fork_and_exec(const char *child_prog, const char *sig_str, const char *opt_str);
void ex3b_send_kill(pid_t pid, int sig);
void ex3b_wait_and_report(pid_t pid);

#endif