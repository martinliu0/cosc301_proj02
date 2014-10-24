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
/*
Author: Martin Liu
*/


int run_command(char **s, int mode, Node **head, Process **p){ //returns the end mode
	int curr_mode = mode;
	int is_mode = -3;
	//int num_commands = count_commands(s);
	//pid_t pid[num_commands];
	//int pid_index =0;
	//Process *process = *p;
	int exit_out = 0;
	int i = 0;
	if (s == NULL){
		return -1;
	}
	while(s[i] != NULL){
		is_mode = find_mode(s[i]);

		if (is_mode != 2){
			if(is_mode != -1 && is_mode != -2){
				curr_mode = is_mode;
				i++;
				continue;
			}
			else{
				i++;
				continue;
			}

		}
		char *commands = strdup(s[i]); 
		char **temp = tokenify(commands, " \t\n");
		if (temp[0] == NULL){
			i++;
			free_tokens(temp);
			free(commands);
			continue;
		}
		if(strcasecmp(temp[0], "exit") == 0){
			exit_out = 1;
			i++;
			free_tokens(temp);
			free(commands);
			continue;
		}
		if(strcasecmp(temp[0], "jobs") == 0){
			jobs(*p);
			i++;
			free_tokens(temp);
			free(commands);
			continue;
		}
	    if(strcasecmp(temp[0], "pause") == 0){
	    	if (temp[1] != NULL){
	    		char *temp1 = strdup(temp[1]);
	    		int temp2 = atoi(temp1);
	    		free(temp1);

		    	pid_t w = (pid_t) temp2;
		    	printf("what is pid: %d\n", w);
		    	if (is_process(w, *p)){
		    		kill(w, SIGSTOP);
		    		new_status(w, "paused", p);
		    		jobs(*p);
		    	}

		    	i++;
				free_tokens(temp);
				free(commands);
		    	continue;    		
	    	}
	    	else {
	    		printf("Please give a pid for command 'pause'.\n");
	    		i++;
				free_tokens(temp);
				free(commands);
		    	continue;    
	    	}	
		}
	    if(strcasecmp(temp[0], "resume") == 0){
	    	if (temp[1] != NULL){
	    		char *temp1 = strdup(temp[1]);
	    		int temp2 = atoi(temp1);
	    		free(temp1);
		    	pid_t w = (pid_t) temp2;
		    	if (is_process(w, *p)){
		    		kill(w, SIGCONT);
		    		new_status(w, "running", p);
		    		jobs(*p);
		    	}	
		    	i++;
				free_tokens(temp);
				free(commands);
		    	continue;        		
	    	}
	    	else {
	    		printf("Please give a pid for command 'resume'.\n");
	    		i++;
				free_tokens(temp);
				free(commands);
		    	continue;    
	    	}	
		}
		temp[0] = test_commands(temp[0], head);

		pid_t pid = fork();
		
		if (pid == 0){//child process
			if(execv(temp[0], temp)<0){
				fprintf(stderr, "execv failed: %s\n", strerror(errno));
				//delete_process(pid, p);
				free_tokens(temp);
				free(commands);
				exit(0);
			}
			
		}
		else{	
			if(mode == 0){//in sequential mode*/
				wait(NULL);
				//pid_index++;
				//printf("what is pid_index: %d\n", pid_index);	
				i++;	
				free(commands);
				free_tokens(temp);
				continue;

			}
			else{
				add_process(pid, s[i], "running", p);
				i++;
				free(commands);
				free_tokens(temp);
				continue;
			}

		}
		free(commands);
		free_tokens(temp);
	}	

/*	if (mode == 1){


	}*/
	if(exit_out){
		return -1;
	}

	return curr_mode;

}
int check_process(Process **p){
	Process *temp = *p;
	int any_completion = 0;
	while(temp != NULL){
		int status = waitpid(temp->p_id, NULL, WNOHANG);
		if (status != 0){
			//printf("what is jobs: %s\n", temp->instr);
			temp->instr[strlen(temp->instr)-1] = '\0';
			printf("Process %d (%s) is completed.\n", temp->p_id, temp->instr);
			fflush(stdout);	
			delete_process(temp->p_id,p);
			jobs(*p);
			any_completion = 1;
		}

		temp = temp->next;
	}
	//printf("what is returned: %d\n", any_completion);
	return any_completion;
}
int main(int argc, char **argv) {
	printf("%s\n","Shell -Intelligenti: to exit, type 'exit'.");
	printf("%s\n", "To run commands in parallel mode, type 'mode p.'");
	printf("%s\n", "To run commands in sequential mode, type 'mode s.'");
	char **command;
	//int *mode_switches;
	int curr_mode = 0; //default mode is sequential
	Node *head = NULL;
	Process *p = NULL;
	FILE *datafile = fopen("shell-config", "r");
	setup_shell(&head, datafile);
	//list_print(head);
	int printprompt = 1;
	fclose(datafile);
	
	while(1){
		if (printprompt){
			print_prompt(curr_mode);
		}
		
		struct pollfd pfd[1];
    	pfd->fd = 0; // stdin is file descriptor 0
   		pfd->events = POLLIN;
    	pfd->revents = 0;
    	int rv = poll(&pfd[0], 1, 100);
		if (rv == 0) {
		   	printprompt = check_process(&p);

		} else if (rv > 0) {
			command = get_prompt();
		    curr_mode = run_command(command, curr_mode, &head, &p);
		    free_tokens(command);
		    printprompt = 1;

		} else {
		    printf("there was some kind of error: %s\n", strerror(errno));
		    free_tokens(command);
			list_clear(head);
		    exit(0);
		}

		//printf("%d\n", printprompt);
		//free_tokens(command);
		
		
		if (curr_mode == -1){
			//free_tokens(command);
			list_clear(head);
			exit(0);
		}
		

	}
	//free(mode_switches);
    // a blank slate :-)
    list_clear(head);
    return 0;
}

