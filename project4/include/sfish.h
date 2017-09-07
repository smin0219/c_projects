#ifndef SFISH_H
#define SFISH_H
#include <readline/readline.h>
#include <readline/history.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <string.h>
#define MAX_SIZE 1024
#define USAGE(return_code) do { \
	fprintf(stdout, "\n%s\n", \
		"help\tPrint a list of all builtins and their basic usage in a signle column.\n" \
		"exit\tExits the shell by using the exit(3) function\n" \
		"cd [dir] [-] [.] [..]\tChanges the current working directory of the shell\n" \
		"pwd\tPrint the absolute path of the current working directory\n"); \
} while (0);
/**
 * @breif Handle builtin commands accordingly
 * @param command Commands that are given by user
 * @param curr_dir Current working directory
 * @param prev_dir Previous working directory
**/

int builtin_command(char** command, char* curr_dir, char* prev_dir, int redir);
/**
 * @breif Change directory according to the options that are provided by user
 * @param option Option for cd
 * @param curr_dir Current working directory
 * @param prev_dir Previous working directory
**/
int change_directory(char* option, char* prev_dir);
/**
 * @breif e
 * @param option Option for cd
 * @param curr_dir Current working directory
 * @param prev_dir Previous working directory
**/
/**
 * @breif Modify prompt of the shell
 * @param shell Current prompt of the shell
 * @param curr_dir Current working directory
**/
void modify_shell(char* shell, char* curr_dir);
int execute(char** cmd,int redir, int pipe);
int redirection(char** command, int* fd, char* dir);

char* find_dir(char** prog, char dir[MAX_SIZE]);

void easy_handler();

void alarm_handler(int signal);

void child_handler();

#endif
