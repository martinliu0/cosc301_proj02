#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <ctype.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <poll.h>
#include <signal.h>
#include <errno.h>
#include <time.h>
#include "functs.h"
#include "list.h"
#include <inttypes.h>
#include <iso646.h>

char** tokenify(const char *s, char *delimiter) {
    char *strcopy = strdup(s); 
    char *token = strtok(strcopy, delimiter);
/*    printf("what is token: %s\n", token);
    if (token == NULL){
        printf("NULSDF]\n");
        free(strcopy);
        char **strarr = malloc(sizeof(s));
        strarr[0] = strdup(s);
        return strarr;
    }*/
    int str_arr_size = sizeof(token);
    while (token != NULL) {
        token = strtok(NULL, delimiter);
        str_arr_size += sizeof(token);
    }
    free(strcopy);
    char *another_copy = strdup(s);
    char *another_token = strtok(another_copy, delimiter);
    char **strarr = malloc(str_arr_size);
    int count = 0;
    while (another_token != NULL) {
        strarr[count] = strdup(another_token);
        another_token = strtok(NULL, delimiter);
        count++;
    }
    free(another_copy);
    strarr[count] = NULL;   
    return strarr; 
}

void free_tokens(char **tokens){
    int i = 0;
    while(tokens[i] != NULL){
        free(tokens[i]);
        i++;
    }
    free(tokens);
}

void replace_pound(char *s){
    for (int i = 0; i < strlen(s); i++){
        if (s[i] == '#' && s[i-1] == ' '){ //ingore comments e.g. ls #123
            s[i-1] = '\0';
            break;
        }
    }
}

void print_prompt(int i){
    if (i == 1){
        printf("%s", "prompt (parallel)> ");
    }
    else{
        printf("%s", "prompt (sequential)> ");
    }
    fflush(stdout);
}
char **get_prompt(){
    char str[1024];
    char **temp;
    if (fgets(str, 1024, stdin) == NULL){
        return NULL;
    }
    replace_pound(str);
    temp = tokenify(str, ";");
    return temp; //remember to free tokens
}
void print_mode(int mode){
    if(mode == 0){
        printf("current mode: sequential\n");
    }
    else{
        printf("curr_moderrent mode: parallel\n");
    }
}

int find_mode(char *s){ 
    char *copy = strdup(s);
    char **mode = tokenify(copy," \t\n");
    if (mode[0]!=NULL){
        if (strcasecmp(mode[0], "mode") == 0){
            if(mode[1] == NULL){
                printf("Invalid mode: '%s'\n", mode[0]);
                free(copy);
                free_tokens(mode);              

                return -1; //-1 is invalid mode input
                
            }
            if(strcasecmp(mode[1], "s")==0){
                free(copy);
                free_tokens(mode);
                
                return 0; // 0 is sequential mode
            }

            else if(strcasecmp(mode[1], "p")==0){
                free(copy);
                free_tokens(mode);
                

                return 1; // 1 is parallel mode
            }

            else{
                
                printf("Invalid mode: '%s'\n", s);
                free(copy);
                free_tokens(mode);
                
                
                return -1; //-1 is invalid mode input
            }
        }
        free(copy);
        free_tokens(mode);
        return 2;
    }

    else{
        free(copy);
        free_tokens(mode);

        return -2; //in the case of "/bin/ls ; ; /bin/ps"
    }

}


char *prepend(char *s, Node **head){
    int i = 0;
    int j = 0;
    char *copy = NULL;
    int k =0;
    if((*head)!= NULL){
        copy = malloc((strlen(s)+strlen((*head)->data)+1)*sizeof(char));
        for(; i < strlen((*head)->data); i++){
            copy[i] = (*head)->data[i];
        }
        k = i;
        for (; k < (i+strlen(s)) ; k ++){
            copy[k] = s[j];
            j++;
        }
    }
    copy[k] = '\0';
    return copy;
}

void setup_shell(Node **head, FILE *shellconfig){
    char line[1024];
   // Node *curr = head;
    while (fgets(line, 1024, shellconfig) != NULL){
        //printf("what is line: %s\n", line);
        line[strlen(line)-1] = '/';
        line[strlen(line)] = '\0';
        insert(line, head);
    }
    //list_print(*head);
    //printf("what is head: %s\n", (*head)->data);
}

char *test_commands(char *s, Node **head){
    Node *curr = *head;
    struct stat sb;
    int rv = stat(s, &sb);
    //printf("what is rv: %d\n", rv);
    char *dir = NULL;
    if (rv < 0){
        while (curr != NULL){
            struct stat pb;
            dir = prepend(s, &curr);
            //printf("what is dir:%s\n", dir);
            rv = stat(dir, &pb);
            if (rv == 0){
                free(s);
                return dir;
            }
            free(dir);
            curr = curr->next;
        }
        return s;
    }
    else{
        return s;
    }

}

void jobs(Process *head){
    //Process *curr = *head;
    if (head == NULL){
        printf("No process at this time.\n");
        return;
    }
    while(head!=NULL){
        printf("Process %d %s %s\n", (head)->p_id, (head)->instr, (head)->status);
        head = head->next;
    }
    
}

void new_status(pid_t id, char *update, Process **head){
    Process *curr = *head;
    while(curr!= NULL){
        if (curr->p_id == id){
            strcpy(curr->status, update);
        }
        curr=curr->next;
    }
}

int is_process(pid_t id, Process *head){
    while(head != NULL){
        if (head->p_id == id){
            return 1;
        }
        head = head->next;
    }
    printf("Bad pid '%d'\n", id);
    return 0;
}
