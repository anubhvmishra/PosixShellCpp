#include<string>
using namespace std;
string trim(const string& line)
{
    const char* WhiteSpace = " \t\v\r\n";
    int start = line.find_first_not_of(WhiteSpace);
    int end = line.find_last_not_of(WhiteSpace);
    return ((start == end)&&(end==-1)) ? string() : line.substr(start, end - start + 1);
}
