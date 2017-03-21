#ifndef __TEST_H__
#define __TEST_H__

#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <cstdlib>
#include <string>
#include <cstring>
#include <vector>
#include <queue>
#include <sstream>
#include <dirent.h>
#include <iostream>

#include "newbase.h"

using namespace std;

class Test: public Base {
    protected:
		char* command; //name of the command
		queue<char*> flags_queue; //vector holding the flags provided

	public:
		Test(char* cmd, queue<char*> flags)
			:	command(cmd), flags_queue(flags) {}
		Test(queue<char*> flags){
			command[0] = '\0';	//empty string
			flags_queue = flags;
		}
		Test(){}

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
			struct stat buf;
			string testFlag;
			string pathToFile;
			bool fileExists = false;

			if(flags_queue.size() == 1) {
				testFlag = "-e";
				pathToFile = flags_queue.front();
			}
			else {
				testFlag = flags_queue.front();
				flags_queue.pop();
				pathToFile = flags_queue.front();
			}

			int statReturnVal = stat(pathToFile.c_str(),&buf);
			if (statReturnVal == 0) {
				fileExists = true;
			}

			if(dup2(output,1) == -1) {
				perror("dup2 failed");
				return false;
			}

			if(statReturnVal == -1) {
				perror("stat failed");
				return false;
			}

			if(testFlag == "-e") {
				cout << "(TRUE)" << endl;
				return fileExists;
			}
			else if(testFlag == "-d") {
				if(S_ISDIR(buf.st_mode)) {
					cout << "(TRUE)" << endl;
					return true;
				}
				else {
					cout << "(FALSE)" << endl;
					return false;
				}
			}
			else if(testFlag == "-f") {
				 if(S_ISREG(buf.st_mode)) {
					 cout << "(TRUE)" << endl;
					 return true;
				 }
				else {
					cout << "(FALSE)" << endl;
					return false;
				}
			}
			else {
				cout << "Error: test flag not valid"  << endl;
				return false;
			}
		}

		
};
#endif
