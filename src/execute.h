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
bool execute(Command* cmd){
    if(strcmp(cmd->getCommand(), "exit") == 0){
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
        if(strcmp(cmd_list.at(i)->getCommand(), "test") == 0){
            struct stat buf;
            if(stat(cmd_list.at(i)->getArguments()[2], &buf) == -1){
                //failed
                lastSuccess = false;
                cout << "(FALSE)" << endl;
            }
            else{ //stat succeeded
                lastSuccess = true;
                if(strcmp(cmd_list.at(i)->getArguments()[1], "-e") == 0){
                    if(S_ISREG(buf.st_mode) || S_ISDIR(buf.st_mode)){
                        //true
                        cout << "(TRUE)" << endl;
                    }
                    else{
                        cout << "(FALSE)" << endl;
                    }
                } 
                else if(strcmp(cmd_list.at(i)->getArguments()[1], "-f") == 0){
                    if(S_ISREG(buf.st_mode)){
                        cout << "(TRUE)" << endl;
                    }
                    else{
                        cout << "(FALSE)" << endl;
                    }
                }
                else if(strcmp(cmd_list.at(i)->getArguments()[1], "-d") == 0){
                    if(S_ISDIR(buf.st_mode)){
                        cout << "(TRUE)" << endl;
                    }
                    else{
                        cout << "(FALSE)" << endl;
                    }
                }
            }
        }
        else if(lastConnector == ';'){
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
