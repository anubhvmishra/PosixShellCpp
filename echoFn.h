#include<vector>
#include<string>
#include<unistd.h>
using namespace std;

/* Implementation of echo
Assumptions:
1) Input has to be enclosed in double quotes
2) Input after the double quotes is ignored*/

int printEchoString(string echoCommand)
{
    char firstChar=echoCommand[0];
    if((firstChar!='"')|| (echoCommand.find_last_of('"')==echoCommand.find_first_of('"')))
    {
        string errMsg="Wrong syntax for echo: Please enclose your input in quotes i.e. echo \"<your input>\"\n";
        write(STDOUT_FILENO,errMsg.c_str(),errMsg.size());
        return 1;
    }
    else
    {
        string toEcho=echoCommand.substr(1,echoCommand.find_last_of('"')-1); //taking string except first quote(")
        toEcho+="\n";
        write(STDOUT_FILENO,toEcho.c_str(),toEcho.size());
        return 0;
    }
}