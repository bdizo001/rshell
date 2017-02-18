#include <vector>
#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <stdlib.h>

#include "parse.h"
#include "command.h"
#include "execute.h"
using namespace std;

int main(){
    while(1){
        //prompt user
        char hostname[128];
        gethostname(hostname, sizeof hostname);
        cout << getlogin() << "@" << hostname << "$ ";

        //get input
        string userInput;
        getline(cin, userInput);        

        //parse userinput into Command objects
        vector<Command*> command_vector;
        parse(userInput, command_vector);

        //execute commands
        executeAll(command_vector);     
    }
    return 0;
}
