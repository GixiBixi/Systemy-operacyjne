#ifndef EX3C_H
#define EX3C_H

#include <sys/types.h>
void ex3c_parse_args_c1(int argc, char *argv[], int *out_sig, int *out_opt, int *out_nchildren);
void ex3c_parse_args_c2(int argc, char *argv[], int *out_sig, int *out_opt, int *out_nchildren); 
void ex3c_parse_args_c3(int argc, char *argv[], int *out_sig, int *out_opt);
pid_t ex3c_fork_and_exec(const char *prog, char *const args[]);
void ex3c_become_leader(void);
void ex3c_ignore_signal(int sig);
void ex3c_send_kill_group(pid_t pgid, int sig);
void ex3c_wait_and_report(pid_t pid);
void ex3c_wait_all(void);
#endif