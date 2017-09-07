#include "sfish.h"
#include "debug.h"
/*
 * As in previous hws the main function must be in its own file!
 */


int main(int argc, char const *argv[], char* envp[]){

	signal(SIGTSTP, SIG_IGN);
	signal(SIGUSR2, easy_handler);
	signal(SIGALRM, alarm_handler);

	//signal()
	/* DO NOT MODIFY THIS. If you do you will get a ZERO. */
	rl_catch_signals = 0;
	/* This is disable readline's default signal handlers, since you are going to install your own.*/
	int count = 0;
	int bg = 0;
	int redir = 0;
	int pipe = 0;
	char* cmd = "fortune | cowsay | cowsay";
	char* token = NULL;
	char* saveptr = NULL;
	char* command[MAX_SIZE];
	char curr_dir[MAX_SIZE];
	char prev_dir[MAX_SIZE];
	char shell[MAX_SIZE];
	char temp[MAX_SIZE];
	//curr_dir = (char*)calloc(MAX_SIZE,sizeof(char*));
	memset(command,0,sizeof(command));
	memset(curr_dir,0,sizeof(curr_dir));
	memset(prev_dir,0,sizeof(prev_dir));
	memset(shell,0,sizeof(shell));
	memset(temp,0,sizeof(temp));
	/* GET CURRENT WORKING DIRECTORY FOR SHELL */
	getcwd(curr_dir,sizeof(curr_dir));
	strcpy(prev_dir,curr_dir);
	/* MODIFY PROMPT OF THE SHELL */
	modify_shell(shell,curr_dir);
	/* MODIFY THE PROMPT OF THE SHELL */
	//shell = (char*)calloc(MAX_SIZE,sizeof(char*));
	//temp = (char*)calloc(strlen(cmd),sizeof(char*));
	//command = (char*)calloc(strlen(cmd),sizeof(char*));
while((cmd = readline(shell)) != NULL) {
		/* IF NO ARGUMENT IS GIVEN */
		if(strcmp(cmd,"")==0){
			fprintf(stderr,"%s\n","No command is given!");
			//error("No command is given!%s","\n");
			free(cmd);
			continue;
		}
		/* CLEAN MEMORY FOR NEW COMMAND */
		memset(temp,0,sizeof(temp));
		memset(command,0,sizeof(command));
		/* SPLIT COMMAND ARGUMENTS TO ANALYZE */
		strcpy(temp,cmd);
		/* GET FIRST ARGUMENT */
		token = strtok_r(temp," ",&saveptr);
		command[count] = token;
		count++;
		/* GET REST ARGUMENTS */
		while((token=strtok_r(saveptr," ",&saveptr))!=NULL){
			command[count] = token;
			count++;
		}
		count = 0;
		/* EXIT COMMAND */
		if (strcmp(command[0], "exit")==0){
			if(command[1] != NULL){
				fprintf(stderr,"%s\n","Invalid");
				free(cmd);
				info("Invalid exit!!%s\n","");
			}
			else{
				info("Valid exit!!%s\n","");
				break;
			}
		}
		redir= 0;
		pipe = 0;
		/* CHECK WHETHER IT NEEDS REDIRECTION OR NOT */
		while(command[count]!=NULL){
			if((strcmp(command[count],"<")==0) || (strcmp(command[count],">")==0 )){
				redir = 1;
			}
			else if (strcmp(command[count],"|")==0){
				redir = 1;
				pipe = 1;
			}
			count++;
		}
		info("pipe%d\n",pipe);
		count = 0;
		/************************************************************************
		**********************************PART 1*********************************
		*************************************************************************/
		bg = builtin_command(command,curr_dir,prev_dir,redir);
		if (strcmp(command[0],"pwd")==0){
			info("pwd!!%s","\n");
			free(cmd);
			return EXIT_SUCCESS;
		}
		if((strcmp(command[0],"cd")==0) && (bg != -1)){
			memset(prev_dir,0,sizeof(prev_dir));
			strcpy(prev_dir,curr_dir);
			memset(curr_dir,0,sizeof(curr_dir));
			getcwd(curr_dir,sizeof(curr_dir));
			modify_shell(shell,curr_dir);
			info("Modify prompt shell!!%s\n","");
		}
		if(bg == -1){
			fprintf(stderr,"%s\n","Invalid Command");
			free(cmd);
		}
		else if(!bg){
			free(cmd);
			info("Builtin command handled properly%s\n","");
		}
		/************************************************************************
		**********************************PART 2*********************************
		*************************************************************************/
		else{
			info("Check executable command!!%s\n","");
			bg = execute(command,redir,pipe);
			info("Executable command handled properly%s\n","");
			if(bg == -1){
				fprintf(stderr,"%s\n","Invalid Command");
				if(redir == 1){
					free(cmd);
					return -1;
				}
			}
			else{
				info("Executable command handled properly%s\n","");
			}
			free(cmd);
		}
	}
	fprintf(stderr,"\n");
	info("NULL is given to readline%s","\n");
	info("Please restert program%s","\n");
	info("Program is terminating%s","\n");
	free(cmd);
	return EXIT_SUCCESS;
}
