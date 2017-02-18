#ifndef EXECUTE_H
#define EXECUTE_H

#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <cstdlib>
using namespace std;

//uses fork, execvp, wait to run commands encapsulated in Command objects
//returns 0 if failed to run command, 1 if successfully run command
int execute(Command* cmd){
    char* exitstr = "exit";
    if(cmd->getCommand() == exitstr){
        exit(1);
    }
    else{
        pid_t pid = fork();

        //error
        if(pid == -1){
            perror("fork");
            return 0;
        }

        //child process, call execvp here
        if(pid == 0){
            if(execvp(cmd->getCommand(), cmd->getArguments()) == -1){  
                perror("exec");
                return 0;
            }
        }

        //parent process, call wait here
        if(pid > 0){
            //wait for child process to finish
            //child processes killed after execvp call
            if(wait(0) == -1){
                perror("wait");
                return 0;
            }
        }   
    }
    return 1;
}

//pass in the vector of Command objects
//determines which commands should be executed
    //; = always execute next cmd
    //&& = execute if previous succeeded
    //|| = execute if previous failed
void executeAll(vector<Command*> cmd_list){
    char lastConnector = ';';       
    bool lastSuccess = true;       
 
    for(unsigned i = 0; i < cmd_list.size(); ++i){
        if(lastConnector == ';'){
            lastSuccess = execute(cmd_list.at(i));
        }

        else if(lastSuccess && lastConnector == '&'){
            lastSuccess = execute(cmd_list.at(i));
        }
    
        else if(!lastSuccess && lastConnector == '|'){
            lastSuccess = execute(cmd_list.at(i));
        }
        lastConnector = cmd_list.at(i)->getConnector();
    }
}
#endif
