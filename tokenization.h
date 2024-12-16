#include<iostream>
#include<string.h>
#include<vector>
using namespace std;
string trim(const string& line)
{
    const char* WhiteSpace = " \t\v\r\n";
    int start = line.find_first_not_of(WhiteSpace);
    int end = line.find_last_not_of(WhiteSpace);
    return ((start == end)&&(end==-1)) ? string() : line.substr(start, end - start + 1);
}
vector<string> getTokens(string s, string delimiters)
{
    vector<string> resultTokens;
    char* inp=s.data();
    char* token = strtok(inp, delimiters.c_str());

    while (token != NULL) 
    {
        
        resultTokens.push_back(trim(token));
        token = strtok(NULL, delimiters.c_str());
    }
    return resultTokens;
    
}