#ifndef PARSE_H
#define PARSE_H

#include <cstring>
#include <cctype>

#include "command.h"
using namespace std;

void parse(string userInput, vector<Command*> &vec, char** args){
    //break string at the connectors
    vector<string> cmd_strings;
    string temp;
    int start = 0;
    for(unsigned i = 0; i < userInput.size(); ++i){
        if(userInput.at(i) == ';'){
            temp = userInput.substr(start, i-start+1);
            if(i != userInput.size()-1){
                if(userInput.at(i+1) == ' '){
                    start = i+2;
                    i = start;
                }
                else{
                    start = i+1;
                    i = start;
                }
            }
            cmd_strings.push_back(temp);               
        }
        else if(userInput.at(i) == '&' || userInput.at(i) == '|'){
            //assume next char is the same &&/||    
            temp = userInput.substr(start, i-start+2);
            if(i != userInput.size()-1){
                if(userInput.at(i+2) == ' '){
                    start = i+3;
                    i = start;
                }
                else{
                    start = i+2;
                    i = start;
                }
            }
            cmd_strings.push_back(temp);
        }
        
        //special case: last command has no trailing connector
        else if(i == userInput.size()-1){
            temp = userInput.substr(start, i-start+1);
            cmd_strings.push_back(temp);        
        } 
    }

    //convert to cstring, fill Command objects
    char connector;
    for(unsigned i = 0; i < cmd_strings.size(); ++i){
        //grab connector
        if( cmd_strings.at(i).at(cmd_strings.size()-1) == ' '){
            connector = cmd_strings.at(i).at(cmd_strings.at(i).size()-2);  
        }
        else{
            connector = cmd_strings.at(i).at(cmd_strings.at(i).size()-1);
        }
        //
        //separate into tokens
        char* current = (char*)cmd_strings.at(i).c_str();
        char * pch;
        int index = 0;
        pch = strtok(current, " ;&|");
        while(pch != NULL){
            args[index] = pch;
            index++;
            pch = strtok(NULL, " ;&|");
        }
        args[index] = NULL;        

        //create command object, populate command vector
        Command* obj = new Command(args[0], args, connector);             
        vec.push_back(obj);
    }

    return;
}

#endif
