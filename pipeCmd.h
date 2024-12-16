#include <string>
void pipeRedir(string command, int stdinFD, int stdoutFD, int& foregroundPID)
{
    int pipeArray[2];
    int writeToPipe=stdoutFD;       
    int readFromPipe=stdinFD;
    vector<string> commandsList=getTokens(command,"|");
    int length=commandsList.size();
    
    for(int i=0;i<length;i++)
    {
        if(dup2(readFromPipe, STDIN_FILENO)==-1)         // STDIN now points to reading end of pipe
        {
            perror("An error occurred while reading from pipe");
            return;
        }
        

        if(i!=length-1) 
        {
            if(pipe(pipeArray)!=0) 
            {
                perror("An error occurred while creating pipe descriptors");
                return;
            }
            writeToPipe=pipeArray[1];
        }
        else 
        {
            writeToPipe=stdoutFD;           //Last command writes to original stdout
        }

        if(dup2(writeToPipe, STDOUT_FILENO)==-1) 
        { 
            perror("An error occurred while writing to pipe");
            return;
        }

        if (i!=0) close(readFromPipe); // Close the read end of the previous pipe
        if (i!=length-1) close(pipeArray[1]); // Close write end of the current pipe in parent process
        
        string toExec=commandsList[i];
        vector<string> arguments=getTokens(toExec," ");
        
        if((toExec.find('<')!=-1)&&(toExec.find('>')==-1))        //only input redirection
        {
            inpRedir(toExec,stdinFD,foregroundPID);
        }
        else if( (toExec.find('<')==-1) && (toExec.find('>')!=-1)) //only output redirection
        {
            outRedir(toExec,stdoutFD,foregroundPID);
        }
        else if((toExec.find('<')!=-1)&&(toExec.find('>')!=-1))   //both input/output redirection
        {
            inoutRedir(toExec,stdinFD,stdoutFD,foregroundPID);
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
                _exit(1);
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
        if (i!=length-1)
        {
            readFromPipe = pipeArray[0]; // The next command reads from the read end of the pipe
        }
    }
    dup2(stdinFD,STDIN_FILENO);
    dup2(stdoutFD,STDOUT_FILENO);
    close(stdinFD);
    close(stdoutFD);
}