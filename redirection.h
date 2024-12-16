void inpRedir(string command, int stdinFD, int& foregroundPID)
{
    string toExec=trim(strtok(command.data(),"<"));
    string nameOfFile=trim(strtok(NULL,"<"));
    if((toExec.compare("")==0)||(nameOfFile.compare("")==0))
    {
        string errMsg="An error occurred. Please follow proper syntax [command] < [filename with extension]\n";
        write(STDOUT_FILENO,errMsg.c_str(),errMsg.size());
    }
    else
    {
        int fd=open(nameOfFile.c_str(),O_RDONLY);
        if(fd==-1)
        {
            perror("Error occurred while opening the file");
        }
        else
        {
            if(dup2(fd,STDIN_FILENO)==-1) 
            {
                perror("Error occurred while changing input source");
                return;
            }
            else
            {
                int pid=fork();
                if(pid==-1)
                {
                    perror("An error occurred while trying to execute the process");
                }
                else if(pid==0)
                {
                    vector<string> arguments=getTokens(toExec," ");
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
                    string cmd=execvpArgs[0];
                    int child=execvp(cmd.c_str(),execvpArgs);
                    if(child==-1)
                    {
                        perror("Invalid Command");
                        _exit(1);
                    }
                    dup2(stdinFD,STDIN_FILENO);
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
                    dup2(stdinFD,STDIN_FILENO);
                }
            }
        }
    }
}

void outRedir(string command, int stdoutFD, int& foregroundPID)
{
    if((command.find('>')!=-1)&&(command.find(">>")==-1))       // overwrite the file
    {
        string toExec=trim(strtok(command.data(),">"));
        string nameOfFile=trim(strtok(NULL,">"));
        if((toExec.compare("")==0)||(nameOfFile.compare("")==0))
        {
            printf("An error occurred. Please follow proper syntax [command] > [filename with extension]\n");
        }
        else
        {
            int fd=open(nameOfFile.c_str(),O_WRONLY | O_CREAT | O_TRUNC,00644);
            if(fd==-1)
            {
                perror("Error occurred while writing to the file");
            }
            else
            {
                if(dup2(fd,STDOUT_FILENO)==-1) 
                {
                    perror("Error occurred while changing output source");
                    return;
                }
                else
                {
                    int pid=fork();
                    if(pid==-1)
                    {
                        perror("An error occurred while trying to execute the process");
                    }
                    else if(pid==0)
                    {
                        vector<string> arguments=getTokens(toExec," ");
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
                        string cmd=execvpArgs[0];
                        int child=execvp(cmd.c_str(),execvpArgs);
                        if(child==-1)
                        {
                            perror("Invalid Command");
                            _exit(1);
                        }
                        dup2(stdoutFD,STDOUT_FILENO);
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
                        dup2(stdoutFD,STDOUT_FILENO);
                    }
                }
            }
        }

    }
    else if(command.find(">>")!=-1)                             // append the file, no need to check presence of > because if >> exists then > automatically exists
    {
        string toExec=trim(strtok(command.data(),">>"));
        string nameOfFile=trim(strtok(NULL,">>"));
        if((toExec.compare("")==0)||(nameOfFile.compare("")==0))
        {
            printf("An error occurred. Please follow proper syntax [command] >> [filename with extension]\n");
        }
        else
        {
            int fd=open(nameOfFile.c_str(),O_WRONLY | O_CREAT | O_APPEND,00644);
            if(fd==-1)
            {
                perror("Error occurred while appending the file");
            }
            else
            {
                if(dup2(fd,STDOUT_FILENO)==-1) 
                {
                    perror("Error occurred while changing output source");
                    return;
                }
                else
                {
                    int pid=fork();
                    if(pid==-1)
                    {
                        perror("An error occurred while trying to execute the process");
                    }
                    else if(pid==0)
                    {
                        vector<string> arguments=getTokens(toExec," ");
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
                        string cmd=execvpArgs[0];
                        int child=execvp(cmd.c_str(),execvpArgs);
                        if(child==-1)
                        {
                            perror("Invalid Command");
                            _exit(1);
                        }
                        dup2(stdoutFD,STDOUT_FILENO);
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
                        dup2(stdoutFD,STDOUT_FILENO);
                    }
                }
            }
        }
    }

}

void inoutRedir(string command, int stdinFD, int stdoutFD, int& foregroundPID)
{
    string toExec=trim(strtok(command.data(),"<"));
    string secondArgument=trim(strtok(NULL,"<"));
    string nameOfInputFile,nameOfOutputFile;
    if(secondArgument.find(">>")==-1)
    {
        nameOfInputFile=trim(strtok(secondArgument.data(),">"));
        nameOfOutputFile=trim(strtok(NULL,">"));
    }
    else
    {
        nameOfInputFile=trim(strtok(secondArgument.data(),">>"));
        nameOfOutputFile=trim(strtok(NULL,">>"));
    } 

    if((toExec.compare("")==0)||(nameOfInputFile.compare("")==0)||(nameOfOutputFile.compare("")==0))
    {
        string errMsg="An error occurred. Please follow proper syntax [command] < [input filename with extension] > [output filename with extension]\n";
        write(STDOUT_FILENO,errMsg.c_str(),errMsg.size());
    }
    else
    {
        int infd=open(nameOfInputFile.c_str(),O_RDONLY);
        int outfd;
        if(secondArgument.find(">>")==-1)
        {
            outfd=open(nameOfOutputFile.c_str(),O_WRONLY | O_CREAT | O_TRUNC,00644);
        }
        else
        {
            outfd=open(nameOfOutputFile.c_str(),O_WRONLY | O_CREAT | O_APPEND,00644);
        }
        if((infd==-1)||(outfd==-1))
        {
            perror("An error occurred while processing inputs");
        }
        else
        {
            if((dup2(infd,STDIN_FILENO)==-1)||(dup2(outfd,STDOUT_FILENO)==-1))
            {
                perror("Error occurred while changing input/output source");
                return;
            }
            else
            {
                int pid=fork();
                if(pid==-1)
                {
                    perror("An error occurred while trying to execute the fork process");
                }
                else if(pid==0)
                {
                    vector<string> arguments=getTokens(toExec," ");
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
                    string cmd=execvpArgs[0];
                    int child=execvp(cmd.c_str(),execvpArgs);
                    if(child==-1)
                    {
                        perror("Invalid Command");
                        _exit(1);
                    }
                    dup2(stdinFD,STDIN_FILENO);
                    dup2(stdoutFD,STDOUT_FILENO);
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
                    dup2(stdinFD,STDIN_FILENO);
                    dup2(stdoutFD,STDOUT_FILENO);
                }
            }
        }
    }
}