#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"

/* your list function definitions */
void list_print(const Node *list) {
    printf("*** List contents begin *** \n");
    while (list != NULL) {
        printf("%s\n", list->data);
        list = list->next;
    }
    printf("*** List contents end *** \n");
}

void list_clear(Node *list) {
    while (list != NULL) {
        Node *tmp = list;
        list = list->next;
        free(tmp);
    }
}


void insert(char *data, Node **head){
    if ((*head) == NULL) {
        Node *newnode = malloc(sizeof(Node));
        strcpy(newnode->data,data);
        //printf("what is new node data0: %s\n", newnode->data);
        newnode->next = NULL;
        *head = newnode; 
    }
    else{
        Node *curr = *head;
        //printf("head in insert: %s\n", (*head)->data);
        while(curr->next != NULL){
            curr = curr->next;
        }

        Node *newnode = malloc(sizeof(Node));
        newnode->next = NULL;
        strcpy(newnode->data,data);
        //printf("what is new node dat
        
        curr->next = newnode;

        //printf("new node data: %s\n", (curr->next)->data);
    }
}

void add_process(pid_t id, char *a, char *b, Process **head){
    if ((*head) == NULL){
        Process *newprocess = malloc(sizeof(Process));
        newprocess->p_id = id;
        strcpy(newprocess->instr, a);
        strcpy(newprocess->status, b);
        newprocess->next = NULL;
        *head = newprocess;
    }
    else{
        Process *curr = *head;
        while(curr->next != NULL){
            curr = curr->next;
        }
        Process *newprocess = malloc(sizeof(Process));
        newprocess->next = NULL;
        newprocess->p_id = id;
        strcpy(newprocess->instr, a);
        strcpy(newprocess->status, b);

        curr->next = newprocess;
    }
}

void delete_process(pid_t id, Process **head){
    Process *newprocess = *head;
    if(newprocess != NULL){
        if (newprocess->p_id == id && newprocess->next == NULL){
            *head = (*head)->next;
            free(newprocess);
            return;
        }

        while(newprocess->next != NULL){
            if ((newprocess->next)->p_id == id){
                Process *temp = newprocess->next;
                newprocess->next = (newprocess->next)->next;
                free(temp);
                return;
            }
            newprocess = newprocess->next;

        }
    }
    
}