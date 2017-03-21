#ifndef __CMD_H__
#define __CMD_H__

#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <cstdlib>
#include <string>
#include <cstring>
#include <queue>

#include "newbase.h"

using namespace std;

class Command: public Base {
    protected:
		char* command;
		queue<char*> flags_queue; //vector holding the flags provided
    public:
		//constructors
		Command(char* cmd, queue<char*> flags)
			:	command(cmd),	flags_queue(flags) {}
		Command(queue<char*> flags){
			command[0] = '\0';	//empty cstring
			flags_queue = flags;
		}
		Command(char* cmd){
			command = cmd;
			queue<char*> empty_queue;
			flags_queue = empty_queue;
		}
		Command(){}

		void add_flag(char* new_flag){
			flags_queue.push(new_flag);
		}
		
		string getString(){
			return command;
		}
		
		//executes the command using the system calls fork
		//execvp and wait returns true if the command is executed
		//and false if it fails
		virtual bool execute(int input, int output){
			flags_queue.push(NULL);
			int count = 1;
			char* args[100];
			while(!flags_queue.empty()){
				args[count] = flags_queue.front();
				flags_queue.pop();
				count += 1;
			}
			
			args[0] = command;
			bool returnVal = true;
			
			pid_t pid = fork();
			if(pid == -1){
				//fork fails
				perror("fork failure");
			}
			else if(pid == 0){
				//child process, do work here
				if(dup2(input, 0) == -1){
					perror("dup2 failed");
					return false;
				}
				if(dup2(output, 1) == -1){
					perror("dup2 failed");
					return false;
				}
				if(execvp(args[0], args) == -1){
					returnVal = false;
					perror("execvp failed");
					exit(1);
				}
			}
			else if(pid > 0){
				//parent process, wait for child
				int status;
				if(waitpid(pid, &status, 0) == -1){
					perror("wait failed");
				}
				if(WEXITSTATUS(status) != 0){
					returnVal = false;
				}
			}
			return returnVal;
		}


};
#endif
