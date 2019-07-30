/*
	CSC 360 Assignment 1
	Alwien Dippenaar
	V00849850
*/

#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>

// Global Variables
#define MAX_INPUT 128
#define TRUE 1
#define FALSE 0

// Linked List Data Structure Functions

struct node {
	pid_t procPid;
	char *process;
	int isExcecuting;
	struct node* next;
};

struct node *head = NULL;

/*
*	@param procPid: The process ID of the new node to be inserted.
*	@param process: The process file that is to be executed.
*	This function will insert a new node into the linked list.
*/
void insert (pid_t procPid, char* process) {
	struct node *newNode = (struct node*)malloc(sizeof(struct node));
	if (newNode != NULL) {
		char *valProc = malloc((strlen(process)+1)*1);
		strcpy(valProc, process);
		newNode->procPid = procPid;
		newNode->process = valProc;
		newNode->isExcecuting = TRUE;
		newNode->next = NULL;
	}
	if (head == NULL) {
		head = newNode;
	} else {
		struct node *temp = head;
		while (temp->next != NULL) {
			temp = temp->next;
		}
		temp->next = newNode;
	}
}

/*
*	@param childPid: The process ID of the process a function wants to find.
*	@return If the process is not in the linked list.
*	@return If the process deleted the node from the linked list.
*	This function will delete a node from the linked list from a given process ID.
*/
void delete(pid_t childPid) {
	if (!isInList(childPid)) { 
		return;		
	}
	struct node *temp = head;
	struct node *temp2 = NULL;
	while (temp != NULL) {
		if (temp->procPid == childPid) {
			if (temp == head) {
				head = head->next;
			} else { 
				temp2->next = temp->next;
			}
			free(temp->process);
			free(temp);
			return;
		}
		temp2 = temp;
		temp = temp->next;
	}
}

/*
*	@param childPid: The process ID of the process a function wants to find.
*	@return TRUE: The process is in the linked list, TRUE == 1.
*	@return FALSE: The process in not in the linked list, FALSE == 0.
*	This function looks to see if a process is in the linked list from a given process ID.
*/
int isInList(pid_t childPid) {
	struct node *temp = head;
	while (temp != NULL) {	
		if (temp->procPid == childPid) { 
			return TRUE;
		}
		temp = temp->next;
	}
	return FALSE;
	
}

/*
*	@param childPid: The process ID of the process a function wants to find.
*	@return temp: The process that is in the linked list that the function is looking for.
*	@return NULL: The process was not found in the linked ist.
*	This function finds a process node in the linked list from a given process ID.
*/
struct node *getNode(pid_t childPid) {
	struct node *temp = head;
	while (temp != NULL) {
		if (temp->procPid == childPid) {
			return temp;
		}
		temp = temp->next;
	}
	return NULL;
}

// End of Linked List Functions

/*
*	This function will check the status of the processes and clean up any
*	that may be in the Z (zombie) state.
*	This function also checks the exit status value of the child process.
*/
void checkProcessStatus() {
	int procStatus;
	pid_t childPid;
	for (;;) {
		childPid = waitpid(-1, &procStatus, WNOHANG);
		if (childPid > 0) {
			if (WIFEXITED(procStatus)) { //process was terminated
				printf("Background process %d was terminated.\n", childPid);
				delete(childPid);				
			}
			if (WIFCONTINUED(procStatus)) { // process has started
				printf("Background process %d was started.\n", childPid);
				struct node *temp = getNode(childPid);
				temp->isExcecuting = TRUE;
			}	
			if (WIFSTOPPED(procStatus)) { // proccess was stopped
				printf("Background process %d was stopped.\n", childPid);
				struct node *temp = getNode(childPid);
				temp->isExcecuting = FALSE;
			}
			if (WIFSIGNALED(procStatus)) { // process was killed
				printf("Background process %d was killed.\n", childPid);
				delete(childPid);
			}	
		} else {
			break;
		}
	}
}


/*
*	@param input: The input from the user for which command they want to excecute
*		along with any other parameters the command require to run successfully
*	This function will run all the commands (bg, bglist, bgkill, bgstop, bgstart, and pstat),
*	if the user types an incorrect command an error message will print to the terminal.
*/
void excecuteCommand(char** input) {
	/*
	*	Will start a background process with a given file name from user input
	*/
	if (strncmp(input[0], "bg", strlen(input[0])) == 0) {
		pid_t childPid = fork();
		if (childPid == 0) { // It is the child
			execvp(input[1], &input[1]);
			perror("Error on execvp");
			exit(1);
		} 
		else if (childPid > 0) { // It is the parent
			printf("Background process started: %d\n", childPid);
			printf("process to input: %s\n", input[1]);
			insert(childPid, input[1]);
		} else {
			perror("Fork");
		}	 
	}
	/*
	*	Will print out the list of processes currently excecuting in the background
	*/
	else if (strncmp(input[0], "bglist", strlen(input[0])) == 0) {
		if (head == NULL) {
			printf("No files currently excecuting in the background.");
		} else {	
			struct node *temp = head;
			int procCounter = 0;
			while (temp != NULL) {	
				procCounter++;
				if (temp->isExcecuting) {
					char cwd[256];
					if (getcwd(cwd, sizeof(cwd)) == NULL) {
						perror("getcwd() Error");
					}
					char *tok = strtok(temp->process, "./");
					printf(" %d:\t %s/%s\n", temp->procPid, cwd, tok);
					temp = temp->next;
				}
			}
			printf("Total Background Jobs: %d", procCounter);
		}
	}
	/*
	*	Will terminate the bakground process with a specified pid from user input
	*/
	else if (strncmp(input[0], "bgkill", strlen(input[0])) == 0) {
		pid_t childPid = atoi(input[1]);
		if (isInList(childPid)) {
			int retval = kill(childPid, SIGTERM);
			if (retval == 0) {
				sleep(1);
			} else {
				printf("Error: Failed to execute bgkill.\n"); 
			}
		} else {
			printf("Error: Process %d does not exist.", childPid);
		}
	}
	/*
	*	Will stop a background process with a specified pid from user input
	*/
	else if (strncmp(input[0], "bgstop", strlen(input[0])) == 0) {
		pid_t childPid = atoi(input[1]);
		if (isInList(childPid)) {
			int retval = kill(childPid, SIGSTOP);
			if (retval == 0) {
				sleep(1);
			 } else {
				printf("Error: Failed to execute bgstop.\n");
			}
		} else {
			printf("Error: Process %d does not exist.", childPid);
		}
	}
	/*
	*	Will start a background process with a specified pid from user input
	*/
	else if (strncmp(input[0], "bgstart", strlen(input[0])+1) == 0) {
		pid_t childPid = atoi(input[1]);
		if (isInList(childPid)) {
			int retval = kill(childPid, SIGCONT);
			if (retval == 0) {
				sleep(1);
			} else {
				printf("Error: Failed to execute bgstart.\n");
			}
		} else {
			printf("Error: Process %d does not exist.", childPid);
		}
	}
	/*
	* 	Will print out a list of required information about a specified process pid from user input
	*/
	else if (strncmp(input[0], "pstat", strlen(input[0])+1) == 0) {
		pid_t childPid = atoi(input[1]);
		if (isInList(childPid)) {
			char procStat[MAX_INPUT];			
			char procStatus[MAX_INPUT];
			sprintf(procStat, "/proc/%d/stat", childPid);
			sprintf(procStatus, "/proc/%d/status", childPid);
		
			char statusContent[MAX_INPUT][MAX_INPUT];
			FILE *statusFile = fopen(procStatus, "r");
			if (statusFile != NULL) {
				int i = 0;
				while (fgets(statusContent[i], MAX_INPUT, statusFile) != NULL) {
					i++;
				}
				fclose(statusFile);
			} else {
				printf("Error: Could not read status file.\n");
				return;
			}			

			char *procStatContent[MAX_INPUT];
			FILE *statFile = fopen(procStat, "r");
			if (statFile != NULL) {
				char fileContent[2048];
				fgets(fileContent, sizeof(fileContent), statFile);
				char *tok = strtok(fileContent, " ");
				int i = 0;
				while (i < MAX_INPUT) {
					procStatContent[i] = tok;
					tok = strtok(NULL, " ");
					i++;
				}
				fclose(statFile);
			} else {
				printf("Error: Could not read stat file.\n");
				return;
			}
			printf("comm: %s\n", procStatContent[1]);
			printf("state: %s\n", procStatContent[2]);
			printf("utime: %lf\n", strtof(procStatContent[13], NULL)/sysconf(_SC_CLK_TCK));
			printf("stime: %lf\n", strtof(procStatContent[14], NULL)/sysconf(_SC_CLK_TCK));
			printf("rss: %s\n", procStatContent[23]);
			printf("voluntary_ctxt_switches: %s", statusContent[39]);
			printf("nonvoluntary_ctxt_switches: %s", statusContent[40]);
		} else {
			printf("Error: Process %d does not exist.", childPid);
		}
	} else {
		printf("PMan: > %s: Command Not Found.\n", input[0]);
	}
}

/*
*	@param input: The pointer to where the users input will be allocated.
*	@return FALSE: There is no actual input to process, FALSE == 0.
*	@return TRUE: The input was processed successfully, TRUE == 1.
*	This function will take in user input and tokenize each word.
*/
int getInput(char** input) {
	char userInput[MAX_INPUT]; 
	printf("\nPMan: > ");
	fgets(userInput, sizeof(userInput), stdin);
	if (strncmp(userInput, "", sizeof(userInput)) == 0) {
		return FALSE;
	}	
	char *tok = strtok(userInput, " \n");
	int i = 0;
	while (i < MAX_INPUT) {
		if (tok != "") {
			input[i] = tok;
		}
		tok = strtok(NULL, " \n");
		i++;
	}
	return TRUE;
}

/*
* 	Main function to constantly ask for user input from the terminal.
*/
int main() {
	for (;;) { 
		char *input[MAX_INPUT];
		int inputSuccess = getInput(input);
		if (inputSuccess) {
			excecuteCommand(input);
		}
		checkProcessStatus();
	}
	return 0;
}
