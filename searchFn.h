#include<sys/types.h>
#include<dirent.h>
#include<vector>
#include<string>
#include<sys/stat.h>
using namespace std;
void printSearchResult(string command)
{
    vector<string> files;

    char wd[1000];
	char* nameOfDir=getcwd(wd,1000);
	DIR* dirp = opendir(nameOfDir);
    struct dirent* dp;
    while ((dp = readdir(dirp)) != NULL) {
        files.push_back(dp->d_name);
    }
    closedir(dirp);

    //Removing . and .. special files to avoid recursion
    auto it = find(files.begin(), files.end(), ".");
	files.erase(it);

	it = find(files.begin(), files.end(), "..");
	files.erase(it);

    struct stat fileProperties;
    string currDir=nameOfDir;
    int length=files.size();

    for(int i=0;i<length;i++)
    {
        stat(files[i].c_str(),&fileProperties);
        if(S_ISDIR(fileProperties.st_mode))
        {
            string currDir=nameOfDir;
            string childDir=currDir+"/"+files[i];

            dirp = opendir(childDir.c_str());
            while ((dp = readdir(dirp)) != NULL) {
                files.push_back(dp->d_name);
            }
            closedir(dirp);
        }
    }

    command=trim(command);
    length=files.size();
    for(int i=0;i<length;i++)
    {
        if(command.compare(files[i])==0)
        {
            write(STDOUT_FILENO,"True\n",5);
            return;
        }
    }
    write(STDOUT_FILENO,"False\n",6);
}