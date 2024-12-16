#include<string>
#include<unistd.h>
#include<sys/stat.h>
#include<fcntl.h>
using namespace std;
void printPinfo(string command)
{
    int pid;
    if(command.size()==5) //only pinfo
    {
        pid=getpid();
    }
    else
    {
        int locOfWhiteSpace=command.find(' ');
        string argument=command.substr(locOfWhiteSpace+1,command.size()-(locOfWhiteSpace+1));
        const char* numbers="0123456789";
        int index=argument.find_first_not_of(numbers);
        if(index!=-1)
        {
            string errMsg="Invalid PID\n";
            write(STDOUT_FILENO,errMsg.c_str(),errMsg.size());
            return;
        }
        else
        {
            pid=stoi(argument);
        }
    }
    
    string procLocation="/proc/"+to_string(pid);

    string execPathLocation=procLocation+"/exe";

    procLocation=procLocation+"/stat";
    struct stat processInfo;
    if(stat(procLocation.c_str(),&processInfo)<0)
    {
        string errMsg="Invalid PID\n";
        write(STDOUT_FILENO,errMsg.c_str(),errMsg.size());
        return;
    }
    else
    {
        char process_stats[100000];
        int fd=open(procLocation.c_str(),O_RDONLY);
        if(fd<0)
        {
            string errMsg="Error while trying to open the proc file\n";
            write(STDOUT_FILENO,errMsg.c_str(),errMsg.size());
            return;
        }
        int rd = read(fd, process_stats, 1e5);
        if(fd<0)
        {
            string errMsg="Error while trying to read the proc file\n";
            write(STDOUT_FILENO,errMsg.c_str(),errMsg.size());
            return;
        }
        vector<string> statOfFile=getTokens(process_stats," ");
        string processStatus=statOfFile[2];
        string virtualMemory=statOfFile[22];

        int getGID=getpgid(pid);                    //to get the group ID of the current process
        int terminalID=tcgetpgrp(STDOUT_FILENO);
        if(getGID==terminalID)
        {
            processStatus+="+";
        }

        string pidPrint="pid -- "+to_string(pid)+"\n";
        write(STDOUT_FILENO,pidPrint.c_str(),pidPrint.size());

        processStatus="Process Status -- "+processStatus+"\n";
        write(STDOUT_FILENO,processStatus.c_str(),processStatus.size());

        virtualMemory="Memory -- "+virtualMemory+" {Virtual Memory}\n";
        write(STDOUT_FILENO,virtualMemory.c_str(),virtualMemory.size());

        // Getting executable path through symbolic links
        char executablePath[100000];
        int rdlink=readlink(execPathLocation.c_str(),executablePath,1e5);
        if(rdlink<0)
        {
            string errMsg="Error while trying to read the symbolic link to executable file\n";
            write(STDOUT_FILENO,errMsg.c_str(),errMsg.size());
            return;
        }

        string exePath=executablePath;
        exePath="Executable Path -- "+exePath+"\n";
        write(STDOUT_FILENO,exePath.c_str(),exePath.size());
    }
}