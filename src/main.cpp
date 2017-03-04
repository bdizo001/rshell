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
    char hostname[128];
    gethostname(hostname, sizeof hostname);
    while(1){
        //prompt user
        cout << getlogin() << "@" << hostname << "$ ";

        //get input
        string userInput;
        getline(cin, userInput);        
        
        //filter out comments, not needed for execution
        userInput = removeComments(userInput);

        //parse userinput into Command objects
        vector<Command*> command_vector;
        char* args[100];
        parse(userInput, command_vector, args);

        //execute commands
        executeAll(command_vector);     
    }
    return 0;
}
