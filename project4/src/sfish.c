#include "sfish.h"
#include "debug.h"
/************************************************************************
**********************************PART 1*********************************
*************************************************************************/
static int y = 0;
static pid_t g_pid = 0;
static clock_t start_time = 0;
static clock_t end_time = 0;

int builtin_command(char** command, char* curr_dir, char* prev_dir, int redir){
	int result = 0;
	int fd[2];
	//char* cmd[MAX_SIZE];
	memset(fd,0,sizeof(fd));
	if (strcmp(command[0], "help")==0){
        if(command[1] != NULL){
        	fprintf(stderr,"%s\n","Too many argument");
            return -1;
        }
        USAGE(EXIT_SUCCESS);
        return 0;
    }
    else if(strcmp(command[0],"alarm")==0){
    	//sigset_t mask;
    	//int i = 0;
    	if(command[2]!=NULL){
    		return -1;
    	}
    	if(command[1]==NULL){
    		return 01;
    	}
    	y = *command[1]-'0';
    	//alarm = i;
    	//fflush(stdout);
    	alarm(y);

    	//sigsuspend(&mask);
    	//fprintf("suspendedfor %d",i);
    	//fflush(stdout);

    	return 0;

	}
    else if (strcmp(command[0], "cd")==0){
    	/* TOO MANY ARGUMENT */
    	if(command[2]!=NULL){
    		return -1;
    	}
		return change_directory(command[1],prev_dir);
   	}
    else if (strcmp(command[0],"pwd")==0){
    	pid_t pid;
    	pid_t r_pid;
    	int status = 0;
    	char print_dir[MAX_SIZE];
    	memset(print_dir,0,sizeof(print_dir));
/****************************************************/
    	pid = fork();
    	g_pid = pid;
		if(pid==0){
			if(redir == 1){
					result = redirection(command,fd,"bin");
					if(result == -1){
						return -1;
					}
				}
			info("Child process on%s\n","");
			getcwd(print_dir,sizeof(print_dir));
			printf("%s\n",print_dir);
			//return 0;
		}
		else{
			//signal(SIGCHLD,child_handler);
			r_pid = waitpid(pid,&status,0);

			info("Waiting for child process to be terminated%s\n","");
			if(pid==-1){
				info("Waitpid error!%s","\n");
				return -1;
			}
			else if(pid==0){
				info("Child process is still running%s","\n");
			}
			else if(pid==r_pid){
				signal(SIGCHLD,child_handler);
				info("Child process is terminating%s","\n");
				info("Parent process back on%s\n","");
			}
			if(WIFEXITED(status)){
				info("Return status from child process=%d\n",WEXITSTATUS(status));
			}
			command[0] = "";
		}
		return 0;
    }
    /* IT IS NOT A BUILT IN COMMAND */
    else{
    	return 1;
    }
	return 0;
}
int change_directory(char* option,char* prev_dir){
	/* NO ARGUMENT/HOME DIRECTORY */
	if(option == NULL){
		chdir(getenv("HOME"));
		return 0;
	}
	/* LAST DIRECTORY */
	else if(strcmp(option,"-")==0){
		chdir(prev_dir);
		return 0;
	}
	/* CURRENT DIRECTORY */
	else if(strcmp(option,".")==0){
		chdir(".");
		return 0;
	}
	/* PREVIOUS DIRECTORY */
	else if(strcmp(option,"..")==0){
		chdir("..");
		return 0;
	}
	else{
		if(chdir(option)==-1){
			fprintf(stderr,"%s\n","No file directory");
			return -1;
		}
		else{
			chdir(option);
			return 0;
		}
	}
}
void modify_shell(char* shell, char* curr_dir){
   	char* str1 = "<sumin> : <";
    char* str2 = "> $ ";
	memset(shell,0,sizeof(MAX_SIZE));
	strcpy(shell,str1);
    strcat(shell,curr_dir);
    strcat(shell,str2);
}
/************************************************************************
**********************************PART 2*********************************
*************************************************************************/
int execute(char** command, int redir, int pipe){
	int isExist = 0;
	int slash = 0;
	int status = 0;
	int count= 0;
	int fd[2];
	int result = 0;
	pid_t pid;
	pid_t r_pid;
	struct stat buf;
	char* token = NULL;
	char* saveptr = NULL;
	char* prog = NULL;
	char* argv[MAX_SIZE];
	//char* env[] = {NULL};
	char cmd[MAX_SIZE];
	char dir[MAX_SIZE];
	char temp[MAX_SIZE];
	memset(fd,0,sizeof(fd));
	memset(argv,0,sizeof(argv));
	memset(cmd,0,sizeof(cmd));
	memset(dir,0,sizeof(dir));
	memset(temp,0,sizeof(temp));
	strcpy(cmd,*command);
	strcpy(temp,*command);
	while(*(cmd+count)!=0){
		if(*(cmd+count)=='/'){
			slash = 1;
		}
		count++;
	}
	count = 0;
	if(strcmp(cmd,"..")==0 || strcmp(cmd,".")==0){
		fprintf(stderr,"%s\n","Command not found");
		return -1;
	}
	if(slash==1){
		token = strtok_r(temp,".",&saveptr);
		if((token = strtok_r(token,"/",&saveptr))!=NULL){
			prog = token;
			while((token = strtok_r(saveptr,"/",&saveptr))!=NULL){
				info("current token: %s\n",token);
				prog = token;
			}
			argv[0] = prog;
			count++;
		}
	}
	else{
		argv[0] = command[0];
		count++;
	}
	while(command[count] != NULL){
		argv[count] = command[count];
		count++;
	}
	count = 0;
	/* EXECUTE W/O PATH ENVIRONMENT */
	if(slash==1){
		isExist = stat(cmd,&buf);
		/* IF THE FILE EXSIT, EXECUTE THE PROG */
		if(isExist == 0){
			pid=fork();
			g_pid = pid;
			if(pid==0){
				if(redir == 1){
					result = redirection(argv,fd,cmd);
					if(result == -1){
						return -1;
					}
				}
				info("Child process on%s\n","");
				info("cmd: %s\n",cmd);
				info("argv: %s\n",*argv);
				isExist = execv(cmd,argv);
				if(isExist == -1){
					error("Error occurred from program execution%s","\n");
					if(redir == 1){
						if(fd[0]!=0){
							close(fd[0]);
						}
						if(fd[1]!=0){
							close(fd[1]);
						}
					}
					return -1;
				}
			}
			else{
				r_pid=waitpid(pid,&status,0);
				if(redir == 1){
					if(fd[0]!=0){
						close(fd[0]);
					}
					if(fd[1]!=0){
						close(fd[1]);
					}
				}
				if(pid==-1){
					fprintf(stderr,"%s\n","waitpid error");
					return -1;
				}
				else if(pid==0){
							info("Child process is still running%s","\n");
					}
				else if(pid==r_pid){
					info("Child process is terminating%s","\n");
					info("Parent process back on%s\n","");
					signal(SIGCHLD,child_handler);
				}
				if(WIFEXITED(status)){
					info("Exit from program, status=%d\n",WEXITSTATUS(status));
				}
			}
			return 0;
		}
		/* IF THE FILE DOES NOT EXIST */
		else{
			fprintf(stderr,"%s\n","No such file or directory");
			return -1;
		}
	}
	else{
		if(find_dir(command,dir)==NULL){
			fprintf(stderr,"%s\n","No such file or directory");
			//error("No such file or directory!%s","\n");
			return -1;
		}
		//result = redirection(argv,fd,dir);
		info("Command exist in this dir: %s\n",token);
		pid = fork();
		g_pid = pid;
		if(pid==0){
			if(redir == 1){
				result = redirection(argv,fd,dir);
				info("WHELER???%s\n","");
				if(result == -1){
					if(redir == 1){
						if(fd[0]!=0){
							close(fd[0]);
						}
						if(fd[1]!=0){
							close(fd[1]);
						}
					}
					return -1;
				}
			}
			info("Child process on%s\n","");
			info("ARGV0: %s\n",argv[0]);
			info("ARGV1: %s\n",argv[1]);
			info("ARGV2: %s\n",argv[2]);
			info("ARGV3: %s\n",argv[3]);
			info("DIR: %s\n",dir);
			isExist = execv(dir,argv);
		}
		else{

			r_pid = waitpid(pid,&status,0);
			if(redir == 1){
				if(fd[0]!=0){
					close(fd[0]);
				}
				if(fd[1]!=0){
					close(fd[1]);
				}
			}
			info("Waiting for child process to be terminated%s\n","");
			if(pid==-1){
				error("Waitpid error!%s","\n");
				return -1;
			}
			else if(pid==0){
					info("Child process is still running%s","\n");
			}
			else if(pid==r_pid){
				signal(SIGCHLD,child_handler);
				info("Child process is terminating%s","\n");
				info("Parent process back on%s\n","");
			}
			if(WIFEXITED(status)){
				info("Return status from child process=%d\n",WEXITSTATUS(status));
			}
		}
		return 0;
	}
}
/************************************************************************
**********************************PART 3*********************************
*************************************************************************/
int redirection(char** command, int* fd, char* dir){
	info("In redirection!!%s","\n");
	pid_t pid;
	int count = 0;
	int num = 0;
	int i = 1;
	int pipeNum = 0;
	int setprog = 0;
	int isFirst = 0;
	int isPipe = 0;
	char* arg1[MAX_SIZE];
	char* arg2[MAX_SIZE];
	char* arg3[MAX_SIZE];
	char* prog[MAX_SIZE];
	char temp[MAX_SIZE];
	memset(temp,0,sizeof(temp));
	memset(prog,0,sizeof(prog));
	memset(arg1,0,sizeof(arg1));
	memset(arg2,0,sizeof(arg2));
	memset(arg3,0,sizeof(arg3));
	//prog[count] = command[count];
	//count++;
	while(command[count]!=NULL){
		if(strcmp(command[count],"<")==0){
			fd[0] = open(command[count+1],O_RDONLY);
			fd[0] = dup2(fd[0],0);
			if(*fd==-1){
				fprintf(stderr,"%s\n","No such file or directory");
				return -1;
			}
			setprog = 1;
		}
		else if(strcmp(command[count],">")==0){
			fd[1] = open(command[count+1],O_WRONLY|O_CREAT|O_TRUNC, 0666);
			fd[1] = dup2(fd[1],1);
			if(fd[1]==-1){
				fprintf(stderr,"%s\n","No such file or directory");
				return -1;
			}
			setprog = 1;
		}
		else if(strcmp(command[count],"|")==0){
			isPipe = 1;
			setprog = 1;
		}
		if(setprog != 1){
			prog[count] = command[count];
		}
		count++;
	}
	if(isPipe==1){
		while(command[num]!=NULL){
			if(num==0){
				if(isFirst==0){
					arg1[0] = command[num];
					num++;
				}
			}
			if(pipeNum>2){
				info("PIPENUM####: %d\n",pipeNum);
				arg3[i] = command[num];
				num++;
				i++;
			}
			if(i==0){
				arg2[i] = command[num];
				num++;
				i++;
			}
			else if((strcmp(command[num],"<")==0)||(strcmp(command[num],">")==0)){
				num++;
			}
			else if(((strcmp(command[num],"<")!=0)||(strcmp(command[num],">")!=0))&&(strcmp(command[num],"|")!=0)){
				if(isFirst == 0){
					arg1[i] = command[num];
				}
				else{
					arg2[i] = command[num];
				}
				i++;
				num++;
			}
			else if(strcmp(command[num],"|")==0){
				isFirst = 1;
				pipeNum++;
				num++;
				i=0;
			}
		}
		info("PIPENUM: %d\n",pipeNum);
		if(pipeNum==1){
			pipe(fd);
			if((pid=fork())==0){
				start_time = clock();
				g_pid = pid;

				dup2(fd[1],1);
				close(fd[0]);
				info("ARG1: %s\n",arg1[0]);
				info("ARG1: %s\n",arg1[1]);
				info("ARG1: %s\n",arg1[2]);
				info("ARG1: %s\n",arg1[3]);
				info("ARG1: %s\n",arg1[4]);
				info("ARG1: %s\n",arg1[5]);
				execv(dir,arg1);
			}
			else{
				dup2(fd[0],0);
				close(fd[1]);
				signal(SIGCHLD,child_handler);
				if(arg2[0]==NULL){
					fprintf(stderr,"%s\n","No such file or directory");
					return 1;
				}
				dir = find_dir(arg2,temp);
				info("ARG2: %s\n",arg2[0]);
				info("ARG2: %s\n",arg2[1]);
				info("ARG2: %s\n",arg2[2]);
				info("ARG2: %s\n",arg2[3]);
				info("ARG2: %s\n",arg2[4]);
				info("ARG2: %s\n",arg2[5]);
				execv(dir,arg2);
			}
			setprog = 1;
		}
		else{

			int fd2[4];

			pipe(fd2);
	//pipe(fd2);
/*
			if((pid=fork())==0){
				//pipe(fd2);
				if((pid=fork())==0){

					dup2(fd[1],1);
					close(fd[0]);
					//close(fd2[1]);
					memset(temp,0,sizeof(temp));
					//dir = find_dir(arg1,temp);
					info("DAR1: %s\n",dir);
					info("ARG1: %s\n",arg1[0]);
					execv(dir,arg1);
				}
				else{
					dup2(fd[0],0);
					close(fd[0]);
					close(fd[1]);
					dup2(fd2[1],1);
					close(fd2[0]);
					//close(fd2[1]);
					memset(temp,0,sizeof(temp));
					dir = find_dir(arg2,temp);
					info("DAR1: %s\n",dir);
					info("ARG1: %s\n",arg2[0]);
					execv(dir,arg2);

				}
				exit(3);
			}
			else{
				dup2(fd2[0],0);
				close(fd2[1]);
				close(fd2[0]);
				memset(temp,0,sizeof(temp));
				dir = find_dir(arg3,temp);
				info("DAR1: %s\n",dir);
				info("ARG1: %s\n",arg3[0]);
				execv(dir,arg3);
			}
			*/

			info("ARG2: %s\n","YOLO");
			//int fd2[2];
			pipe(fd);
			if((pid=fork())==0){
				dup2(fd[1],1);
				close(fd[0]);
				close(fd[1]);
				info("DAR1: %s\n",dir);
				info("ARG1: %s\n",arg1[0]);
				//dir = find_dir(arg1,temp);
				execv(dir,arg1);
			}

			pipe(fd2);

			if((pid=fork())==0){
				dup2(fd[0],0);
				dup2(fd2[1],1);

				close(fd[0]);
				close(fd[1]);
				close(fd2[0]);
				close(fd2[1]);

				dir = find_dir(arg2,temp);
				info("DAR2: %s\n",dir);
				info("ARG2: %s\n",arg2[0]);
				execv(dir,arg2);
			}

			close(fd[0]);
			close(fd[1]);

			if((pid=fork())==0){
				dup2(fd2[0],0);
				close(fd2[0]);
				close(fd2[1]);

				memset(temp,0,sizeof(temp));
				dir = find_dir(arg3,temp);
				info("DAR3: %s\n",dir);
				info("ARG3: %s\n",arg1[0]);
				execv(dir,arg3);
			}

		}
	}
	memset(command,0,sizeof(char*)*MAX_SIZE);
	count = 0;
	while(prog[count]!=NULL){
		command[count] = prog[count];
		count++;
	}
	return 0;
}
/************************************************************************
******************************HELPER FUNCTIONS***************************
*************************************************************************/
char* find_dir(char** prog, char dir[MAX_SIZE]){
	struct stat buf;
	int isExist = 0;
	char* path = NULL;
	char* temp = NULL;
	char *token = NULL;
	char *saveptr = NULL;
	path = getenv("PATH");
	temp = (char*)malloc(sizeof(char*)*MAX_SIZE);
	strcpy(temp,path);
	token = strtok_r(temp,":",&saveptr);
	strcpy(dir,token);
	strcat(dir,"/");
	strcat(dir,prog[0]);
	isExist = stat(dir,&buf);
	while(isExist!=0){
		token = strtok_r(saveptr,":",&saveptr);
		memset(dir,0,sizeof(MAX_SIZE));
		if(token==NULL){
			free(temp);
			return NULL;
		}
		strcpy(dir,token);
		strcat(dir,"/");
		strcat(dir,prog[0]);
		isExist = stat(dir,&buf);
		if(isExist==-1){
			token = NULL;
		}
	}
	free(temp);
	return dir;
}
void alarm_handler(int signal){
	fprintf(stdout,"Your %i second timer has finished!\n",y);
}

void easy_handler(){
	fprintf(stderr,"%s","Well that was easy.\n");
}

void child_handler(){
	clock_t diff_time = clock() - start_time;
	end_time = diff_time*1000/CLOCKS_PER_SEC;
	fprintf(stdout,"Child widh PID %i has died. It spent %e milliseconds\n",g_pid,(double)diff_time);
	fflush(stdout);
}
