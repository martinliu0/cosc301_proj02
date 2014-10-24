#ifndef __FUNCTS_H__
#define __FUNCTS_H__
#include "list.h"
char** tokenify(const char *, char *);
char **get_prompt();
void replace_pound(char *);
void free_tokens(char **);
void print_mode(int);
int find_mode(char *);
//int find_exit(char **);
void setup_shell(Node **, FILE *);
char *test_commands(char *, Node **);
char *prepend(char *, Node **);
void jobs(Process *);
void new_status(pid_t, char *, Process **);
void print_prompt(int );
int is_process(pid_t, Process *);
#endif