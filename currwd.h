#include<unistd.h>	// for getcwd()
#include<stdio.h>	// for perror()
#include<climits>	// for INT_MAX
using namespace std;

const int maxLen=1e7;
char wd[maxLen];
int get_currwd()
{
	auto status=getcwd(wd,maxLen);
	if(status==nullptr)
	{
		perror("Error while trying to fetch current directory");
		return 1;
	}
	return 0;
}