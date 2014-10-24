#ifndef __LIST_H__
#define __LIST_H__

/* your list data structure declarations */

typedef struct node {
    char data[1024]; 
    struct node *next;
} Node;

typedef struct Process {
	pid_t p_id;
	char instr[1024];
	char status[1024];
	struct Process *next;
} Process;
/* your function declarations associated with the list */
void list_clear(Node *list);
void insert(char *, Node **);
void list_print(const Node *);
void add_process(pid_t, char *, char *, Process **);
void delete_process(pid_t,Process **);
#endif // __LIST_H__
