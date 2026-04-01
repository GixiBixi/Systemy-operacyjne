#ifndef EX3A_H
#define EX3A_H
#define OPT_DEFAULT  0 
#define OPT_IGNORE   1 
#define OPT_CUSTOM   2 
void ex3a_parse_args(int argc, char *argv[], int *out_sig, int *out_opt);
void ex3a_setup_signal(int sig, int opt);
void ex3a_sighandler(int sig);
void ex3a_run(int sig, int opt);
#endif