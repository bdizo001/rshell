#ifndef COMMAND_H
#define COMMAND_H

#include <string>
using namespace std;
class Command{
    private:
        char* command;      //executable
        char** arguments;   
        char connector;  //|| or &&
    public:
        //Constructors
        Command();
        Command(char* cmd, char** args, char conn)
            : command(cmd), arguments(args), connector(conn) {}

        //Set functions
        void setCommand(char* cmd){ command = cmd; }
        void setArguments(char** args){ arguments = args; }
        void setConnector(char conn){ connector = conn; }

        //Get functions
        char* getCommand(){ return command; }
        char** getArguments(){ return arguments; }
        char getConnector(){ return connector; }
};

#endif
