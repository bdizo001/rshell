#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <cstdlib>
#include <string>
#include <cstring>
#include <vector>
#include <iostream>
#include <queue>
#include <stack>
#include <iterator>

using namespace std;

#include "newbase.h"
#include "command.h"
#include "subConnector.h"
#include "exit.h"
#include "test.h"

//Global connector symbols
const string AND_SYMBOL = "&&";
const string OR_SYMBOL = "||";
const string SEMI_SYMBOL = ";";
const string INPUT_SYMBOL = "<";
const string OUTPUT_SYMBOL = ">";
const string OUTPUT2_SYMBOL = ">>";
const string PIPE_SYMBOL = "|";

//deletes cstring's first character
void delFirst(char* arr) {

    string t = string(arr);
    t = t.substr(1, t.size()-1);
    strcpy(arr, t.c_str());
}

//deletes cstring's last char, does not delete '#' and ';'
void delLast(char* arr) {
    string t = string(arr);
    int hashtag = t.find('#');
    int comma = t.find(';');
    if(hashtag!=std::string::npos || comma!=std::string::npos) {
        t.erase(t.end()-2);
    }
    else {
        t.erase(t.end()-1);
    }
    strcpy(arr, t.c_str());
}

//always deletes the last char
void elimLast(char *arr) {
    string t = string(arr);
    t.erase(t.end()-1);
    strcpy(arr, t.c_str());
}

//checks for connector
bool isConnector(char *arr) {
    vector<string> str;
    str.push_back("&&");
    str.push_back("||");
    str.push_back("|");
    str.push_back(">");
    str.push_back(">>");
    str.push_back("<");
	
    //check if container matches with any connectors
    for(unsigned i=0; i<str.size(); i++)
        if(arr == str.at(i))
            return false;
    return true;
}

//deletes connectors to make newly split commands
string SplitCommands(char *arr) {
    string s = string(arr);
    string output = "";

    for(string::iterator it = s.begin(); it < s.end(); it++) {
        if(*it == '<') {
            output = output +  " < ";
        }
        else if(*it == '>') {
            string::iterator t = it;
            t++;
            if(*t == '>') {
                output = output +  " >> ";
                it = t;
            }
            else {
                output = output +  " > ";
            }
        }
        else if(*it == '|') {
            string::iterator t = it;
            t++;
            if(*t == '|') {
                output = output +  " || ";
                it = t;
            }
            else {
                output = output +  " | ";
            }
        }
        else if(*it == ')') {

        }
        else {
            output = output +  *it;
        }
    }
    return output;
}

//Checks all the connectors
bool checkCon(char *q) {
    vector<string> s;
    s.push_back("&&");
    s.push_back("||");
    s.push_back("#");
    s.push_back(";");

    s.push_back("|"); //Pipe
    s.push_back(">");
    s.push_back(">>");
    s.push_back("<");

    for(unsigned i=0; i<s.size(); i++)
        if(q == s.at(i))
            return false;
    return true;
}

//Function will take in a cstr and return a cntr representation of said cstr.
Base* createContainer(char *cs) {
    //Checking for input/output redirection and piping
    string StringCstr = string(cs);

    int specInput = StringCstr.find('<');
    int specout = StringCstr.find('>');
    int specPipe = StringCstr.find('|');
    if(specInput!=std::string::npos || specout!=std::string::npos || specPipe!=std::string::npos) {
        StringCstr = SplitCommands(cs);
        strcpy(cs, StringCstr.c_str());
    }

    queue<Base *> comList;               //Separates the Commands to create Cmd's objects respectively
    queue<char *> conList;            //Same here, but for Connectors ofc ^
    queue<Connector *> runList;  //This queue should only contain one cntr of all the commmands
                                            //and connectors
	char *tokz = strtok(cs, " ");                                //Initialize a array of Tokens

    //Checking first Argument for semi and comment
    bool firstArgSemi = false; //used within the while loop
    char *checkingSemi = (char *) memchr(tokz, ';', strlen(tokz));          //check first Token for Semi
    char *isComment = (char *) memchr(tokz, '#', strlen(tokz));       //check first Token for Comment
    char *checkingTest = (char *) memchr(tokz, '[', strlen(tokz));          //check first Token for Test
    string checkingStringTest = string(tokz);                            //checks for 'test'

    if(checkingSemi != NULL) {                                        //if 1st Token == ';'
        //parse the Token to not include the ';'
        string tempP = string(tokz);
        tempP = tempP.substr(0, tempP.size()-1);
        strcpy(tokz, tempP.c_str());

        //pushes the ';' into the ConnectorList
        string semiCol = ";";
        char *pushConnector = new char[2];
        strcpy(pushConnector, semiCol.c_str());
        conList.push(pushConnector);

        //Set the boolean to true
        firstArgSemi = true;
    }

    //checks if first string is 'test'
    if(checkingStringTest.compare("test") == 0) {
        tokz = strtok(NULL, " "); //skip the actual string 'test'
        Test *c = new Test();  //Creates a Test instance

        char *checkFlag = (char *) memchr(tokz, '-', strlen(tokz));   //Checks if the next element is a flag
        if(checkFlag != NULL) { //if so take in the flag into account by adding two elements to the flag.
            //cout << tokz << endl;
            c->add_flag(tokz);
            tokz = strtok(NULL, " ");
            c->add_flag(tokz);
            tokz = strtok(NULL, " ");
        }
        else {                  //if not only add one element to the flag
            c->add_flag(tokz);
            tokz = strtok(NULL, " ");
        }

        comList.push(c);   //Push the finish Test into the queue to be turned to a cntr later

        if(tokz != 0) {           //Check before going into the loop
            bool ifConnector = checkCon(tokz);
            char *comHold = (char *) memchr(tokz, '#', strlen(tokz));

            if(comHold != NULL) {
                isComment = comHold;
            }
            else if(ifConnector) {
                cout << "Error: Expected a connector, Received: " << tokz << endl;
                exit(1);
            }
            else {
                conList.push(tokz);
                tokz = strtok(NULL, " ");
            }
        }
    }

    //Used to check '[' same algorithm as the 'test'
    if(checkingTest != NULL) {
        Test *c = new Test();

        tokz = strtok(NULL, " ");

        char *checkFlag = (char *) memchr(tokz, '-', strlen(tokz));
        if(checkFlag != NULL) {
            c->add_flag(tokz);
            tokz = strtok(NULL, " ");
            c->add_flag(tokz);
            tokz = strtok(NULL, " ");
        }
        else {
            c->add_flag(tokz);
            tokz = strtok(NULL, " ");
        }
        //cout << tokz << endl;
        if(tokz != 0) {    //A throw statement for error user checking
            char *checkingEndTest = (char *) memchr(tokz, ']', strlen(tokz));
            if(checkingEndTest == NULL) {
                cout << "Error: Expected: ']', Received: " << tokz << endl;
                exit(1);
            }
        }
        tokz = strtok(NULL, " ");
        //cout << tokz << endl;
        comList.push(c);

        if(tokz!=0) {
            bool ifConnector = checkCon(tokz);
            if(ifConnector) {
                cout << "Error: Expected a connector, Received: " << tokz << endl;
                exit(1);
            }
            char *comHold = (char *) memchr(tokz, '#', strlen(tokz));
            if(comHold != NULL) {
                isComment = comHold;
            }
            else {
                conList.push(tokz);
                tokz = strtok(NULL, " ");
            }
        }
    }


    if(isComment == NULL) {    //if there is a '#' in the first Token, disregard everything
        bool isComment = false;
        while(tokz!=0 && !isComment) {
            //Check if the next token contains a #
            char *comHold = (char *) memchr(tokz, '#', strlen(tokz));
            if(comHold != NULL) {
                isComment = true;
                break;
            }

            string checkingStringTest = string(tokz);  //Same algorithm as commented on top

            if(checkingStringTest.compare("test") == 0) {
                tokz = strtok(NULL, " ");
                Test *c = new Test();


                char *checkFlag = (char *) memchr(tokz, '-', strlen(tokz));
                if(checkFlag != NULL) {
                    c->add_flag(tokz);
                    tokz = strtok(NULL, " ");
                    c->add_flag(tokz);
                    tokz = strtok(NULL, " ");
                }
                else {
                    c->add_flag(tokz);
                    tokz = strtok(NULL, " ");
                }
                comList.push(c);

                if(tokz != 0) {
                    bool ifConnector = checkCon(tokz);
                    char *comHold = (char *) memchr(tokz, '#', strlen(tokz));

                    if(comHold != NULL) {
                        isComment = comHold;
                    }
                    else if(ifConnector) {
                        cout << "Error: Expected a connector, Received: " << tokz << endl;
                        exit(1);
                    }
                    else {
                        conList.push(tokz);
                        tokz = strtok(NULL, " ");
                    }
                }
                else {
                    break;
                }
            }

            char *testingTest = (char *) memchr(tokz, '[', strlen(tokz)); //Same algorithm as top
            if(testingTest != NULL) {
                Test *c = new Test();

				tokz = strtok(NULL, " ");

				for(unsigned i=0; i<2; i++) {
				    c->add_flag(tokz);
				    tokz = strtok(NULL, " ");
				}

                if(tokz != 0) {
                    char *checkingEndTest = (char *) memchr(tokz, ']', strlen(tokz));
				    if(checkingEndTest == NULL) {
				        cout << "Error: Expected: ']', Received: " << tokz << endl;
				        exit(1);
				    }
				}
				tokz = strtok(NULL, " ");
				comList.push(c);

				if(tokz!=0) {
				    bool ifConnector = checkCon(tokz);
				    if(ifConnector) {
				        cout << "Error: Expected a connector, Received: " << tokz << endl;
				    }
				    char *comHold = (char *) memchr(tokz, '#', strlen(tokz));
				    if(comHold != NULL) {
				        isComment = comHold;
				    }
				    else {
				        conList.push(tokz);
				        tokz = strtok(NULL, " ");
				    }
				}
                else {
                    break;
                }
            }

            //Assume that the first Token is always a command thus make a Cmd Object
            Command *testingCommand = new Command(tokz);
            char *q = tokz;

            if(!firstArgSemi && !isComment) {
                q = strtok(NULL, " ");  //Move on to check for the flags

                while(q!=0 && !isComment) {   //Keep checking for connectors while add flags.
                    bool checkConnectors = isConnector(q);     //check if Token is a connector

                    char *comment = (char *) memchr(q, '#', strlen(q)); //Special check for hashtag
                    if(comment != NULL) {           //Disregard everything if comment is found
                        isComment = true;
                    }
                    else {
                        if(checkConnectors) {       //If token is not a Connector
                            char *semi = (char *) memchr(q, ';', strlen(q));    //Check if ';' is in Token
                            char *com = (char *) memchr(q, '#', strlen(q));     //Check if '#' is in Token

                            //If so do the same algorithm except add the flag ';' to Cmd testingCommand
                            if(semi != NULL) {
                                string tempQ = string(q);
                                tempQ = tempQ.substr(0, tempQ.size()-1);
                                strcpy(q, tempQ.c_str());

                                testingCommand->add_flag(q);

                                string Colon = ";";
                                char *pushColon = new char[2];
                                strcpy(pushColon, Colon.c_str());
                                conList.push(pushColon);
                                break;
                            }
                            if(com != NULL) {
                                cout << "# found!" << endl;
                                isComment = true;
                                break;
                            }
                            //other wise just add the whole token as a flag
                            else {
                                testingCommand->add_flag(q);
                            }
                        }
                        else {
                            //If token is a Connector
                            char *com1 = (char *) memchr(q, '#', strlen(q));     //Check if '#' is in Token
                            if(com1 != NULL) {
                                cout << "# found!" << endl;
                                isComment = true;
                                break;
                            }
                            if(!checkConnectors) {
                                conList.push(q);  //push the token into the conList
                            }
                            break;
                        }
                    }
                    q = strtok(NULL, " ");  //Advance through until you find a connector or reach the end
                }//End of while
                tokz = q;                  //q is already checked, set tokz to q
                tokz = strtok(NULL, " ");  //Then advance tokz
                if(testingCommand->getString() == "exit") {    //If the current token is an exit
                    Exit *out = new Exit();                     //Create a Exit Object
                    comList.push(out);                      //push it to the comList
                }                                               //Instead of pushing the testingCommand
                else {
                    comList.push(testingCommand);           //Otherwise just push the testingCommand
                }
            }
            else {  //if the first token had a ';'
                comList.push(testingCommand);               //Then push the new command w/o ';'
                tokz = q;                                          //Advance tokz
                tokz = strtok(NULL, " ");
                firstArgSemi = false;                           //reset the firstArgSemi condiction
            }
        }
        //Construction of cntr execution
        if(conList.size() > 0) { //Only runs when there are 2 or more commands
            //Pop the first two commands out of the comList
            Base *lhs = comList.front();
            comList.pop();
            Base *rhs = comList.front();
            comList.pop();

            //Pop a connector of the conList
            char *t = conList.front();
            conList.pop();

            //Check what the connector is and create a object respectively
            //and push it to the runList
            if(t == AND_SYMBOL) {
                And *x = new And(lhs, rhs);
                runList.push(x);
            }
            else if(t == OR_SYMBOL) {
                Or *x = new Or(lhs, rhs);
                runList.push(x);
            }
            else if(t == SEMI_SYMBOL) {
                Semicolon *x = new Semicolon(lhs, rhs);
                runList.push(x);
            }
            else if(t == INPUT_SYMBOL) {
                input_redirection *x = new input_redirection(lhs, rhs);
                runList.push(x);
            }
            else if(t == OUTPUT_SYMBOL) {
                output_redirection *x = new output_redirection(lhs, rhs);
                runList.push(x);
            }
            else if(t == OUTPUT2_SYMBOL) {
                output_redirection2 *x = new output_redirection2(lhs, rhs);
                runList.push(x);
            }
            else if(t == PIPE_SYMBOL) {
                Pipe *x = new Pipe(lhs, rhs);
                runList.push(x);
            }

            //If there are more connectors, pop out the one command from the runList
            //and set that as the left leaf and pop out another command from the comList
            //and another connector from the conList.
            //
            //create a new Connector object with the runList object as the left leaf,
            //the comList object as the right leaf and the conList object as the parent
            while(conList.size() != 0) {
                Connector *Tlhs = runList.front();
                runList.pop();
                Base *rhs = comList.front();
                comList.pop();

                char *tmp2 = conList.front();
                conList.pop();
                if(tmp2 == AND_SYMBOL) {
                    And *x = new And(Tlhs, rhs);
                    runList.push(x);
                }
                else if(tmp2 == OR_SYMBOL) {
                    Or *x = new Or(Tlhs, rhs);
                    runList.push(x);
                }
                else if(tmp2 == SEMI_SYMBOL) {
                    Semicolon *x = new Semicolon(Tlhs, rhs);
                    runList.push(x);
                }
                else if(tmp2 == INPUT_SYMBOL) {
                    input_redirection *x = new input_redirection(Tlhs, rhs);
                    runList.push(x);
                }
                else if(tmp2 == OUTPUT_SYMBOL) {
                    output_redirection *x = new output_redirection(Tlhs, rhs);
                    runList.push(x);
                }
                else if(tmp2 == OUTPUT2_SYMBOL) {
                    output_redirection2 *x = new output_redirection2(Tlhs, rhs);
                    runList.push(x);
                }
                else if(tmp2 == PIPE_SYMBOL) {
                    Pipe *x = new Pipe(Tlhs, rhs);
                    runList.push(x);
                }
            }
            //After building the cntr, pop the single command from runList
            //and run execute on the command. The execute function will travel through the completed cntr
            //and execute the individual Cmd objects respectively and return a boolean value based on
            //whether it passes or fails.
            Connector *run1 = runList.front();
            runList.pop();
            return run1;
        }
        else {  //If there are no connectors then there must only be one cmd and/or a bunch of flags
            if(comList.size() != 1) {   //MAKE SURE THAT THERE IS ONLY ONE CMD
                //Throw and error if so
                cout << "Error comList has more than 1 Cmd*" << endl;
                exit(1);
            }
            else {
                //Simply pop the one command from the CommandList and execute it
                Base *temporaryCmd = comList.front();
                comList.pop();
                return temporaryCmd;
            }
        }
    }//End of infinite for loop
    if(comList.size() > 1) {
        cout << "Error comList has more than 1 Cmd*" << endl;
        exit(1);
    }
    else {
        Base* cmd = comList.front();
        comList.pop();
        return cmd;
    }
    return NULL;
}

int main() 
{
    while(1) 
	{
        char *userName = getlogin(); //print bash with user tag info
        char hostName[256];
        gethostname(hostName, sizeof hostName); 
        cout << userName << "@" <<  hostName << "$ ";

        string Input;                      
        getline(cin, Input);                

        if(Input == "exit") 
		{               //check for exit!
            Exit *exit = new Exit();
            exit->execute(0,1);
            break;
        }
        char *cs = new char[Input.size()+1];         
        strcpy(cs, Input.c_str());                            
        queue<Base *> precedenceTrees;
        queue<Connector *> outsideConnectors;
        int dPar = Input.find('(');             
        int dTest = Input.find('[');                     
        int dOut = Input.find('>');
        int dInput = Input.find('<');
        int dPipe = Input.find('|');
        if(dPar!=std::string::npos 
		|| (dPar!=std::string::npos && dTest!=std::string::npos) 
		|| (dPar!=std::string::npos 
			&& (dOut!=std::string::npos 
			&& dInput!=std::string::npos 
			&& dPipe!=std::string::npos))) 
			{    
            string out = "";
            char *tokz = strtok(cs, " ");
        
            while(tokz!=0) 							//separate operatorss
			{
                char *begPar = (char *) memchr(tokz, '(', strlen(tokz));
                char *endPar = (char *) memchr(tokz, ')', strlen(tokz));
                char *isTB = (char *) memchr(tokz, '[', strlen(tokz));
                char *isTE = (char *) memchr(tokz, ']', strlen(tokz));
                char *isChInput = (char *) memchr(tokz, '<', strlen(tokz));
                char *isChPipe = (char *) memchr(tokz, '|', strlen(tokz));
                char *isChOut = (char *) memchr(tokz, '>', strlen(tokz));
                int lastPar = 0;
                if(begPar != NULL && (isChInput != NULL || isChPipe != NULL || isChOut!= NULL)) 
				{
                    while(begPar != NULL) 
					{
                        out = out +  "( ";
                        delFirst(tokz);
                        begPar = (char *) memchr(tokz, '(', strlen(tokz));
                    }
                    string pString = SplitCommands(tokz);
                    out = out +  pString;
                    while(endPar != NULL) 
					{
                        lastPar++;
                        delLast(tokz);
                        endPar = (char *) memchr(tokz, ')', strlen(tokz));
                    }
                    char *isComma = (char *) memchr(tokz, ';', strlen(tokz));
                    char *isComment = (char *) memchr(tokz, '#', strlen(tokz));
                    if(isComma != NULL || isComment != NULL) 
					{
                        elimLast(tokz);
                        out = out +  string(tokz);
                        out = out +  " ";
                        for(int i=0; i<lastPar; i++) 
						{
                            out = out +  ") ";
                        }
                        if(isComma != NULL) 
						{
                            out = out +  "; ";
                        }
                    }
                    else {
                        for(int i=0; i<lastPar; i++) 
						{
                            out = out +  " ) ";
                        }
                    }
                }
                else if(begPar != NULL && isTB != NULL && isTE != NULL) 
				{
                    while(begPar != NULL) 
					{
                        out = out +  "( ";
                        delFirst(tokz);
                        begPar = (char *) memchr(tokz, '(', strlen(tokz));
                    }

                    out = out +  "[ ";
                    delFirst(tokz);

                    while(endPar != NULL) 
					{
                        lastPar++;
                        delLast(tokz);
                        endPar = (char *) memchr(tokz, ')', strlen(tokz));
                    }
                    char *isComma = (char *) memchr(tokz, ';', strlen(tokz));
                    char *isComment = (char *) memchr(tokz, '#', strlen(tokz));
                    if(isComma != NULL || isComment != NULL) 
					{
                        elimLast(tokz);
                        out = out +  string(tokz);
                        out = out +  " ";
                        for(int i=0; i<lastPar; i++) 
						{
                            out = out +  ") ";
                        }
                        if(isComma != NULL) 
						{
                            out = out +  "; ";
                        }
                        else {
                            out = out +  "# ";
                        }
                    }
                    else 
					{
                        elimLast(tokz);
                        out = out +  string(tokz);
                        out = out +  " ";
                        out = out +  "] ";
                        for(int i=0; i<lastPar; i++) 
						{
                            out = out +  ") ";
                        }
                    }

                }
                else if(isChInput != NULL || isChOut != NULL || isChPipe != NULL) 
				{
                    string pString = SplitCommands(tokz);

                    out = out +  pString;

                    while(endPar != NULL) 
					{
                        lastPar++;
                        delLast(tokz);
                        endPar = (char *) memchr(tokz, ')', strlen(tokz));
                    }

                    char *isComma = (char *) memchr(tokz, ';', strlen(tokz));
                    char *isComment = (char *) memchr(tokz, '#', strlen(tokz));

                    if(isComma != NULL || isComment != NULL) 
					{
                        elimLast(tokz);
                        out = out +  string(tokz);
                        out = out +  " ";
                        for(int i=0; i<lastPar; i++) 
						{
                            out = out +  ") ";
                        }
                        if(isComma != NULL) 
						{
                            out = out +  "; ";
                        }
                    }
                    else {
                        for(int i=0; i<lastPar; i++) 
						{
                            out = out +  " ) ";
                        }
                    }
                }
                else if(begPar != NULL && isTB != NULL) 
				{
                    while(begPar != NULL) 
					{
                        out = out +  "( ";
                        begPar = (char *) memchr(tokz, '(', strlen(tokz));
                        delFirst(tokz);
                    }
                    out = out +  "[ ";
                    out = out +  string(tokz);
                    out = out +  " ";
                }
                else if(isTB != NULL && isTE != NULL) 
				{
                    out = out +  "[ ";
                    delFirst(tokz);
                    delLast(tokz);
                    out = out +  string(tokz);
                    out = out +  " ";
                    out = out +  "] ";
                }
                else if(isTB != NULL) 
				{
                    out = out +  "[ ";
                    delFirst(tokz);
                    out = out +  string(tokz);
                    out = out +  " ";
                }
                else if(isTE != NULL) 
				{
                    delLast(tokz);

                    out = out +  string(tokz);
                    out = out +  " ";
                    out = out +  "] ";
                }
                else if(begPar != NULL) 
				{
                    while(begPar != NULL) 
					{
                        out = out +  "( ";
                        delFirst(tokz);
                        begPar = (char *) memchr(tokz, '(', strlen(tokz));
                    }
                    out = out +  string(tokz);
                    out = out +  " ";
                }
                else if(endPar != NULL) 
				{
                    while(endPar != NULL) 
					{
                        lastPar++;
                        delLast(tokz);
                        endPar = (char *) memchr(tokz, ')', strlen(tokz));
                    }

                    char *isComma = (char *) memchr(tokz, ';', strlen(tokz));
                    char *isComment = (char *) memchr(tokz, '#', strlen(tokz));

                    if(isComma != NULL || isComment != NULL) 
					{
                        elimLast(tokz);
                        out = out +  string(tokz);
                        out = out +  " ";
                        for(int i=0; i<lastPar; i++) 
						{
                            out = out +  ") ";
                        }
                        if(isComma != NULL) 
						{
                            out = out +  "; ";
                        }
                        else {
                            out = out +  "# ";
                        }
                    }
                    else {
                        out = out +  string(tokz);
                        out = out +  " ";
                        for(int i=0; i<lastPar; i++) 
						{
                            out = out +  ") ";
                        }
                    }
                }
                else 
				{
                    out = out +  string(tokz);
                    out = out +  " ";
                }

                tokz = strtok(NULL, " ");
            }
			//string now contains separated string from operators

            char *allCh = new char[out.size()+1];                  //new cstring 
            strcpy(allCh, out.c_str());                            //separate cstr
            char *c = strtok(allCh, " ");
	        stack<char *> sstack;                              //initialize new containers
	        queue<string> boxes;
	        queue<char *> connectors;
	        bool inPar = false;                         
	        bool newPar = false;                             

			//determines what is within the precedence operators
	        while(c!=0) 
			{
	            char *startPar = (char *) memchr(c, '(', strlen(c));
	            char *stopPar = (char *) memchr(c, ')', strlen(c));
	            bool checkConnectors = checkCon(c);     

	            if(startPar != NULL) 
				{
	                inPar = true;
	                if(!newPar) 
					{
	                    stack<char *> currStr;
	                    while(!sstack.empty()) 
						{
	                        char *a = sstack.top();
	                        char *checksStartPar = (char *) memchr(a, '(', strlen(a));

	                        if(checksStartPar != NULL) 
							{
	                            sstack.pop();
	                            break;
	                        }

	                        currStr.push(a);
	                        sstack.pop();
	                    }
	                    string finStr;

	                    while(!currStr.empty()) 
						{
	                        finStr = finStr +  currStr.top();
	                        finStr = finStr +  " ";
	                        currStr.pop();
	                    }
	                    if(finStr.size() > 0) 
						{
	                        boxes.push(finStr);
	                    }
	                }

	                newPar = true;
	            }
	            else if(stopPar != NULL) 
				{
	                inPar = false;
	                newPar = false;
	                stack<char *> currStr;

	                while(!sstack.empty()) 
					{
	                    char *a = sstack.top();
	                    char *checksStartPar = (char *) memchr(a, '(', strlen(a));
	                    if(checksStartPar != NULL) 
						{
	                        sstack.pop();
	                        break;
	                    }
	                    currStr.push(a);
	                    sstack.pop();
	                }
	                string finStr;

	                while(!currStr.empty()) 
					{
	                    finStr = finStr +  currStr.top();
	                    finStr = finStr +  " ";
	                    currStr.pop();
	                }
	                if(finStr.size() > 0) 
					{
	                    boxes.push(finStr);
	                }
	            }
	            else if(!checkConnectors && !inPar) 
				{
	                connectors.push(c);
	            }
	            else 
				{
	                sstack.push(c);
	            }
	            c = strtok(NULL, " ");
	        }

            //reverse order of string
	        stack<char *> currRem;
	        while(!sstack.empty()) 
			{
	            currRem.push(sstack.top());
	            sstack.pop();
	        }

            //handles remaining string contents
	        string rem;
	        while(!currRem.empty()) 
			{
	            rem = rem +  currRem.top();
	            rem = rem +  " ";
	            currRem.pop();
	        }

	        if(rem.size() != 0) 
			{
	            boxes.push(rem);
	        }
            queue<Base *> cmdContainer;
			//go thru containers and builds objects to be added to the cmdContainer
	        while(!boxes.empty()) 
			{
	            char *r = new char[boxes.front().size()+1];
	            strcpy(r, boxes.front().c_str());

	            boxes.pop();
	            Base* cntr = createContainer(r);
                if(cntr != NULL) 
				{
                    cmdContainer.push(cntr);
	            }
	        }

            if(connectors.size() > 0) 
			{
                queue<Connector *>runList;

                Base* lhs = cmdContainer.front();
                cmdContainer.pop();
                Base* rhs = cmdContainer.front();
                cmdContainer.pop();

                char *t = connectors.front();
                connectors.pop();

                if(t == AND_SYMBOL) 
				{
                    And *x = new And(lhs, rhs);
                    runList.push(x);
                }
                else if(t == OR_SYMBOL) 
				{
                    Or *x = new Or(lhs, rhs);
                    runList.push(x);
                }
                else if(t == SEMI_SYMBOL) 
				{
                    Semicolon *x = new Semicolon(lhs, rhs);
                    runList.push(x);
                }
                else if(t == INPUT_SYMBOL) 
				{
                    input_redirection *x = new input_redirection(lhs, rhs);
                    runList.push(x);
                }
                else if(t == OUTPUT_SYMBOL) 
				{
                    output_redirection *x = new output_redirection(lhs, rhs);
                    runList.push(x);
                }
                else if(t == OUTPUT2_SYMBOL) 
				{
                    output_redirection2 *x = new output_redirection2(lhs, rhs);
                    runList.push(x);
                }
                else if(t == PIPE_SYMBOL) 
				{
                    Pipe *x = new Pipe(lhs, rhs);
                    runList.push(x);
                }
                while(connectors.size() > 0) 
				{
                    Connector *Tlhs = runList.front();
                    runList.pop();
                    Base *rhs = cmdContainer.front();
                    cmdContainer.pop();
                    char *tmp2 = connectors.front();
                    connectors.pop();
                    if(tmp2 == AND_SYMBOL) 
					{
                        And *x = new And(Tlhs, rhs);
                        runList.push(x);
                    }
                    else if(tmp2 == OR_SYMBOL) 
					{
                        Or *x = new Or(Tlhs, rhs);
                        runList.push(x);
                    }
                    else if(tmp2 == SEMI_SYMBOL) 
					{
                        Semicolon *x = new Semicolon(Tlhs, rhs);
                        runList.push(x);
                    }
                    else if(tmp2 == INPUT_SYMBOL) 
					{
                        input_redirection *x = new input_redirection(Tlhs, rhs);
                        runList.push(x);
                    }
                    else if(tmp2 == OUTPUT_SYMBOL) 
					{
                        output_redirection *x = new output_redirection(Tlhs, rhs);
                        runList.push(x);
                    }
                    else if(tmp2 == OUTPUT2_SYMBOL) 
					{
                        output_redirection2 *x = new output_redirection2(Tlhs, rhs);
                        runList.push(x);
                    }
                    else if(tmp2 == PIPE_SYMBOL) 
					{
                        Pipe *x = new Pipe(Tlhs, rhs);
                        runList.push(x);
                    }
                }
                Connector *run1 = runList.front();
                runList.pop();
                run1->execute(0,1);
           }
           else 
		   {
                Base* s = cmdContainer.front();
                cmdContainer.pop();

                if(cmdContainer.size() != 0) 
				{
                    cout << "Error: cmdContainer.size() != 0" << endl;
                    exit(1);
                }
                s->execute(0,1);
            }
        }
        else 
		{
            //special case: only test command called by user
            int dTest = Input.find('[');
            if(dTest!=std::string::npos) 
			{
                string out = "";
                char *tokz = strtok(cs, " ");
                while(tokz!=0) 
				{
                    char *isTB = (char *) memchr(tokz, '[', strlen(tokz));
                    char *isTE = (char *) memchr(tokz, ']', strlen(tokz));

                    //special case: brackets used instead of "test"
                    if(isTB != NULL && isTE != NULL) 
					{
                        out = out +  "[ ";
                        delFirst(tokz);
                        delLast(tokz);
                        out = out +  string(tokz);
                        out = out +  " ";
                        out = out +  "] ";
                    }
                    else if(isTB != NULL) 
					{
                        out = out +  "[ ";
                        delFirst(tokz);
                        out = out +  string(tokz);
                        out = out +  " ";
                    }
                    else if(isTE != NULL) 
					{
                        delLast(tokz);
                        out = out +  string(tokz);
                        out = out +  " ";
                        out = out +  "] ";
                    }
                    else {
                        out = out +  string(tokz);
                        out = out +  " ";
                    }
                    tokz = strtok(NULL, " ");
                }
	            char *r = new char[out.size()+1];
	            strcpy(r, out.c_str());

                Base *s = createContainer(r);
                s->execute(0,1);
            }
            else 
			{
                Base* s = createContainer(cs);
                s->execute(0,1);
            }
        }
        delete[] cs; 
    }
    return 0;
}
