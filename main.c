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

char** tokenify(const char *s, char delimits[]) { //returns array of tokens from string
	char *copy = strdup(s);
	char **tokens = malloc(sizeof(copy)*(strlen(copy)+1));
	char *token = NULL;
	char *temp1 = strtok(copy,delimits);
	if (temp1 != NULL){
		token = temp1;
	}else{
		token = NULL;
	}
	int i = 0;
	while(token!=NULL){
		token = strdup(token);
		tokens[i] = token;
		i++;
		token = strtok(NULL,delimits);
	}
	tokens[i] = NULL;
	free(copy);
	return tokens;
		
}

int count_delim(const char *str, char* substr){
	int count = 0;
	int row = 0; 
	int match = 0;
	int len = strlen(str);
	for(int i = 0; i < len; i++){
		match = strncmp(&str[i],substr,strlen(substr));
		if((match==0)&&(!row)){
			row = 1;
			count++;
		}else if (match!=0){
			row = 0;
		}
	}
	return count;
}

int mode_switch(int mode,char** args){
	if (args[1]) {
		if ((strcmp(args[1],"s")==0)||(strcmp(args[1],"sequential") == 0)){
			return 0;
		}else if ((strcmp(args[1],"p")==0)||(strcmp(args[1],"parallel") == 0)){
			return 1;
		}else{
			printf("error: invalid mode entered\n");
		}
	}else{
		printf("Current mode: ");
		if (mode == 0){
			printf("sequential\n");
		}else{
			printf("parallel\n");
		}
	}return mode;
}

void exec_args(char **args,int mode){ //execs commands, waits if sequential mode
	int pid = fork();
	int status = 0;
	if (pid == 0){
		if((execv(args[0],args) == -1)&&(strcmp(args[0],"mode")!=0)&&(strcmp(args[0],"exit")!=0)){ //if not found and isn't one of our commands
			printf("error: %s not executed\n",args[0]);
		}
		exit(0);
	}else if (mode == 0){
		wait(&status);
	}
}

int has_notspace(char *str){
	for(int i = 0; i < strlen(str); i++){
		if (!isspace(str[i])){
			return 1;
		}
	}
	return 0;
}

int main(int argc, char **argv) {

    char *line = malloc(1024*sizeof(char)); 
    int mode = 0; //0 = sequential, 1 = parallel
    int tmpmode = 0;
    while (1) {
	printf(">");
	int status = 0;
	char* eof = fgets(line,1024,stdin);
	if(eof==NULL){
		printf("\n");
		free(line);
		exit(0);
	}

	char *comment = strchr(line,'#'); //comments
	if (comment != NULL) {
		line[((int)(comment - line))] = '\0';
	}

	char **commands = tokenify(line,";");
	int num_functions = count_delim(line,";") + 1;
	int do_exit = 0;
	for(int i = 0; i < num_functions; i++){
		if ((commands[i]!=NULL)&&(has_notspace(commands[i]))){
			char **args = tokenify(commands[i]," \t\n");
			if (strstr(args[0],"mode") != NULL){
				tmpmode = mode_switch(mode,args); //will take effect after
			}else if (strstr(args[0],"exit") != NULL){
				do_exit = 1;
			}else{
				exec_args(args,mode);
			}
			for(int j = 0; args[j]; j++){
				free(args[j]);
			}
			free(args);
		}
		free(commands[i]);
	}
	free(commands);

	if (mode == 1){ //waiting for parallel mode
		for(int i = 0; i < num_functions; i++){
			wait(&status);
		}
	}
	mode = tmpmode;

	if (do_exit){ //exiting
		free(line);
		exit(0);
	}
    }
    free(line);
    return 0;
}

