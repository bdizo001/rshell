#ifndef __EXIT_H__
#define __EXIT_H__

#include "command.h"

class Exit: public Command {
    public:
		Exit(){}
		virtual bool execute(int input, int output){
			exit(0);
		}
		virtual string getString(){
			return "Exit";
		}
};

#endif
