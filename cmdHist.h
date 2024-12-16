#include<iostream>
#include<string>
#include<vector>
#include<cstdio>
using namespace std;

vector<string> cmdHistory;
int nOfElements=-1;
void setHistory(string cmd)
{
    if(nOfElements==19)
    {
        cmdHistory.erase(cmdHistory.begin());
        nOfElements--;
    }
    cmdHistory.push_back(cmd);
    nOfElements++;
    
}

string getHistory(int i)
{
    if(nOfElements-i>=0) 
    {
        return cmdHistory[nOfElements-i];
    }
    else
    {
        return "";
    }
}

void printHistory(int n)
{
    int toPrint=min(n-1,nOfElements);
    for(int i=nOfElements;i>=(nOfElements-toPrint);i--)
    {
        string commandToPrint=cmdHistory[i]+"\n";
        write(STDOUT_FILENO,commandToPrint.c_str(),commandToPrint.size());
    }
}

void readFromFile()
{
    FILE *prevHistory;
    prevHistory = fopen("history.txt", "r");
    if(prevHistory==nullptr)
    {
        prevHistory = fopen("history.txt", "w+");
    }
    string word="";
    while(true)
    {
        char ch=fgetc(prevHistory);
        if(feof(prevHistory))
        {
            break;
        }
        if(ch=='\n')
        {
            cmdHistory.push_back(word);
            nOfElements++;
            word="";
        }
        else
        {
            word=word+ch;
        }
    }
    if(word.compare("")!=0)
    {
        cmdHistory.push_back(word);
        nOfElements++;
    }
    fclose(prevHistory);
}

void writeToFile(string homePath)
{
    FILE *prevHistory;
    string dest=homePath+"/history.txt";
    prevHistory = fopen(dest.c_str(), "w");
    for(int i=0;i<cmdHistory.size();i++)
    {
        string toAdd=cmdHistory[i];
        fprintf(prevHistory,"%s\n",toAdd.c_str());
    }
    fclose(prevHistory);
}