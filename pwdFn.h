#include<string>

using namespace std;
/*
This function returns the current working directory
*/
void printPwdString()
{
    int get_curr_wd=get_currwd();
    if(get_curr_wd!=0)
    {
        _exit(1);
    }
    string toPrint=wd;
    toPrint+="\n";
    write(STDOUT_FILENO,toPrint.c_str(),toPrint.size());
}