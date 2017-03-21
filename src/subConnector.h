#ifndef SUBCONNECTOR_H
#define SUBCONNECTOR_H

//declares and defines subclasses that inherit from connector class

#include "newbase.h"

#include <stdio.h>
#include <fstream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <cstdlib>
#include <fcntl.h>
#include <string>
using namespace std;

// -----AND CLASS-----
class And: public Connector {
    public:
		//constructors
		And(Base* left, Base* right)
			:	Connector(left, right) {}
		And()
			:	Connector() {}
			
		virtual bool execute(int input, int output){
		//if left executes, execute right also
		//if left fails, dont run right
			if(lhs->execute(input, output)){
				if(rhs->execute(input, output)){	
					return true;
				}
				return false;
			}
			return false;			
		}
};
// -----END AND CLASS-----
//
//
//
// -----OR CLASS-----
class Or: public Connector {
    public:
		Or(Base* left, Base* right)
			:	Connector(left, right) {}
		Or()
			:	Connector() {}
		
		virtual bool execute(int input, int output){
		//if left fails, execute right
		//if left executes, dont execute right
			if(!lhs->execute(input, output)){
				if(rhs->execute(input, output)){
					return true;
				}
				return false;
			}
			return true;
		}
};
// -----END OR CLASS-----
//
//
//
// -----SEMICOLON CLASS-----
class Semicolon: public Connector {
    public:
		//constructors
		Semicolon(Base* left, Base* right)
			:	Connector(left, right) {}
		Semicolon()
			:	Connector() {}

		virtual bool execute(int input, int output){
			//always execute both
			lhs->execute(input, output);
			if(rhs->execute(input, output)){
				return true;
			}
			return false;
		}
};
// ------END SEMICOLON CLASS-----
//
//
//
// ------INPUT REDIRECTION------
class input_redirection: public Connector {
	public:
		input_redirection(Base* left, Base* right)
			:	Connector(left, right) {}
		input_redirection()
			:	Connector() {}
		virtual bool execute(int input, int output){
			string infile = rhs->getString();
			input = open(infile.c_str(), O_RDONLY);
			return lhs->execute(input, 1);
		}
};
// ------END INPUT REDIRECTION------
//
//
//
// -----OUTPUT REDIRECTION SINGLE------
class output_redirection: public Connector{
	public:
		output_redirection(Base* left, Base* right)
			:	Connector(left, right) {}
		output_redirection()
			:	Connector() {}
		virtual bool execute(int input, int output){
			string outfile = rhs->getString();
			output = open(outfile.c_str(), O_WRONLY| O_WRONLY| O_TRUNC | O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);
			return lhs->execute(0, output);
		}
};
// ------END OUTPUT REDIRECTION SINGLE------
//
//
//
// -----OUTPUT REDIRECTION DOUBLE-----
class output_redirection2: public Connector{
	public:
		output_redirection2(Base* left, Base* right)
			:	Connector(left, right) {}
		output_redirection2()
			:	Connector() {}
		virtual bool execute(int input, int output){
			string outfile = rhs->getString();
			output = open(outfile.c_str(), O_WRONLY | O_APPEND | O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);
			return lhs->execute(0, output);
		}
};
// -----END OUTPUT REDIRECTION DOUBLE-----
//
//
//
// -----PIPE CLASS-----
class Pipe: public Connector{
	public:
		Pipe(Base* left, Base* right)
			:	Connector(left, right) {}
		Pipe()
			:	Connector() {}
		virtual bool execute(int input, int output){
			int fds[2];
			if(pipe(fds) == -1){
				perror("pipe");
				return false;
			}
			if(!lhs->execute(input, fds[1])){
				return false;
			}
			
			close(fds[1]);
			
			if(!rhs->execute(fds[0], output)){
				return false;
			}
			close(fds[0]);
			return true;
		}
};
// -----END PIPE CLASS-----
#endif
