#include<unistd.h>          // for geteuid(), gethostname()     
#include<pwd.h>             // for getpwuid()
#include<stdio.h>           // for scanf printf
#include<iostream>
#include<sys/wait.h>        //for waitpid
#include<signal.h>



#include "currwd.h"
#include "tabSugg.h"
#include "tokenization.h"
#include "echoFn.h"
#include "pwdFn.h"
#include "pInfoFn.h"
#include "searchFn.h"
#include "handleLSFn.h"
#include "redirection.h"
#include "pipeCmd.h"

using namespace std;


int foregroundPID=0;
int backrounfPGrp;
void handleSIGINT(int signal)
{
    kill(foregroundPID,SIGINT);
}

void handleSIGTSTP(int signal)
{
    tcsetpgrp(foregroundPID,backrounfPGrp);
    kill(foregroundPID,SIGTSTP);
}


int main()
{
    /*0 and 1 denote STDIN_FILENO and STDOUT_FILENO referring to stdin and stdout so when we do 
    dup2(0,STDIN_FILENO) we are not changing anything. 0 means STDIN_FILENO only. It is not the file desriptor of stdin,
    It is stdin.
    */
    int stdinFD = dup(STDIN_FILENO);
    int stdoutFD = dup(STDOUT_FILENO);

    // Signal handling
    backrounfPGrp=tcgetpgrp(STDIN_FILENO);
    signal(SIGINT,handleSIGINT);
    signal(SIGTSTP,handleSIGTSTP);
    //getting hostname and username
    struct passwd *pw;
    long uid = geteuid();
    pw = getpwuid(uid);
    char hostname[1024];

    tcgetattr(STDIN_FILENO,&old_attributes);
	new_attributes=old_attributes;
	new_attributes.c_lflag=new_attributes.c_lflag & (~ICANON) & (~ECHO);
	tcsetattr(STDIN_FILENO,TCSANOW,&new_attributes);
    
    if(!pw)
    {
        printf("Cannot find username for uid: %ld\n",uid);
        _exit(1);
    }
    else
    {
        readFromFile();
        gethostname(hostname,1024);

        //getting current working directory from currwd.h header file
        int get_curr_wd=get_currwd();
        if(get_curr_wd!=0)
        {
            _exit(1);
        }
        //
        string homePath=wd;
        bool noError=true;
        string previousDir=wd;
        string username=pw->pw_name;
        string shellPrompt=username+"@"+hostname+":~>";

        int flagToExit=0;
        while(noError)
        {
            input.clear();
            
            int exitCondition=getTabSuggestion(shellPrompt);
            if(exitCondition==1)
            {
                break;
            }
            if(!input.empty())
            {
                vector<string> fullCommands=getTokens(input,";");
                for(string command:fullCommands)
                {
                    stdinFD = dup(STDIN_FILENO);
                    stdoutFD = dup(STDOUT_FILENO);
                    if((command.find('|')!=-1))
                    {
                        pipeRedir(command,stdinFD,stdoutFD,foregroundPID);
                    }
                    else if((command.find('<')!=-1)&&(command.find('>')==-1))        //only input redirection
                    {
                        inpRedir(command,stdinFD,foregroundPID);
                    }
                    else if( (command.find('<')==-1) && (command.find('>')!=-1)) //only output redirection
                    {
                        outRedir(command,stdoutFD,foregroundPID);
                    }
                    else if((command.find('<')!=-1)&&(command.find('>')!=-1))   //both input/output redirection
                    {
                        inoutRedir(command,stdinFD,stdoutFD,foregroundPID);
                    }
                    else
                    {
                        if(command.compare("")==0)
                        {
                            string errMsg="An error occurred while parsing the string. Please recheck your commands.\n";
                            write(STDOUT_FILENO,errMsg.c_str(),errMsg.size());
                            continue;
                        }
                        string dupCommand=command;
                        string shellCommand= strtok(dupCommand.data(), " ");       //taking command name

                        vector<string> arguments=getTokens(command," ");        //taking all arguments
                        
                        if(shellCommand.compare("echo")==0)
                        {
                            if(command.size()<5)
                            {
                                string errMsg="Please provide an input for echo command\n";
                                write(STDOUT_FILENO,errMsg.c_str(),errMsg.size());
                            }
                            else
                            {
                                printEchoString(command.substr(5,command.size()-5));
                            }
                        }
                        else if(shellCommand.compare("pwd")==0)
                        {
                            printPwdString();
                        }
                        else if(shellCommand.compare("pinfo")==0)
                        {
                            printPinfo(command);
                        }
                        else if(shellCommand.compare("search")==0)
                        {
                            if(command.size()<7)
                            {
                                string errMsg="Please provide an input for the search command\n";
                                write(STDOUT_FILENO,errMsg.c_str(),errMsg.size());
                            }
                            else
                            {
                                printSearchResult(command.substr(7,command.size()-7));
                            }
                            
                        }
                        else if(shellCommand.compare("history")==0)
                        {
                            int nOfArgs=arguments.size();
                            if(nOfArgs==1)
                            {
                                printHistory(10);
                            }
                            else if(nOfArgs==2)
                            {
                                string histArg=arguments[1];
                                const char* numbers="0123456789";
                                int index=histArg.find_first_not_of(numbers);
                                if(index!=-1)
                                {
                                    string errMsg="Invalid argument: It should be a positive integer\n";
                                    write(STDOUT_FILENO,errMsg.c_str(),errMsg.size());
                                }
                                else
                                {
                                    int intArg=stoi(histArg);
                                    printHistory(intArg);
                                }
                            }
                            else
                            {
                                string errMsg="Invalid number of arguments given for history command\n";
                                write(STDOUT_FILENO,errMsg.c_str(),errMsg.size());
                            }
                        }
                        else if(shellCommand.compare("cd")==0)
                        {
                            int nOfArgs=arguments.size();
                            if(nOfArgs==1)
                            {
                                shellPrompt=username+"@"+hostname+":~>";
                                previousDir=wd;
                                chdir(homePath.c_str());
                                get_curr_wd=get_currwd();
                                if(get_curr_wd!=0)
                                {
                                    _exit(1);
                                }
                            }
                            else if(nOfArgs>2)
                            {
                                string errMsg="cd: Too many arguments\n";
                                write(STDOUT_FILENO,errMsg.c_str(),errMsg.size());
                            }
                            else
                            {
                                if(arguments[1].compare("~")==0)
                                {
                                    arguments[1]=homePath;
                                }
                                if(arguments[1].compare("-")==0)
                                {
                                    arguments[1]=previousDir;
                                }
                                previousDir=wd;
                                int cdir=chdir(arguments[1].c_str());
                                if(cdir==-1)
                                {
                                    perror("An error occurred while trying to change the directory");
                                }
                                else
                                {
                                    get_curr_wd=get_currwd();
                                    if(get_curr_wd!=0)
                                    {
                                        _exit(1);
                                    }
                                    string newPath=wd;
                                    if(homePath.compare(newPath)==0)
                                    {
                                        shellPrompt=username+"@"+hostname+":~>";
                                    }
                                    else
                                    {
                                        int posOfHomePath=newPath.find(homePath);
                                        if(posOfHomePath==-1)
                                        {
                                            shellPrompt=username+"@"+hostname+":~"+newPath+">";
                                        }
                                        else
                                        {
                                            int startFrom=posOfHomePath+homePath.size();
                                            int sizeOfPath=newPath.size()-startFrom;
                                            string afterHomePath=newPath.substr(startFrom,sizeOfPath);
                                            shellPrompt=username+"@"+hostname+":~"+afterHomePath+">";
                                        }

                                    }
                                }

                            }
                        }
                        else if(shellCommand.compare("exit")==0)
                        {
                            flagToExit=1;
                        }
                        else if(shellCommand.compare("ls")==0)
                        {
                            handleLS(arguments,homePath);
                        }
                        else
                        {
                            int pid=fork();
                            if(pid==-1)
                            {
                                perror("An error occurred while trying to execute the process");
                            }
                            else if(pid==0) //child code
                            {
                                int length=arguments.size();
                                char* execvpArgs[length+1];
                                int i;
                                for(i=0;i<length;i++)
                                {
                                    if(arguments[i].compare("&")==0)
                                    {
                                        break;
                                    }
                                    execvpArgs[i]=arguments[i].data();
                                }
                                execvpArgs[i]=nullptr;
                                int child=execvp(shellCommand.c_str(),execvpArgs);
                                if(child==-1)
                                {
                                    perror("Invalid Command");
                                    _exit(1);
                                }
                                
                            }
                            else    //parent
                            {
                                if(command.find('&')!=-1)
                                {
                                    printf("%d\n",pid);
                                }
                                else
                                {
                                    foregroundPID=pid;
                                    int exitStatus;
                                    int pidOfChild=waitpid(pid,&exitStatus,WUNTRACED);
                                    if(pidOfChild==-1)
                                    {
                                        perror("An error occurred");
                                    }
                                } 
                            }
                            
                        }
                    }
                }
            }
            
            writeToFile(homePath);
            if(flagToExit==1)
            {
                break;
            }
        }
    }
    tcsetattr(STDIN_FILENO,TCSANOW,&old_attributes);
}