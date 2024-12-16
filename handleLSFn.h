#include<grp.h>

string getFilename(string path)
{

	int lastIndexOf=path.find_last_of('/');
	if(lastIndexOf==-1)
	{
		return path.c_str();
	}
	string fileName=path.substr(lastIndexOf+1,path.size()-lastIndexOf-1);
	return fileName.c_str();
}


void printFileProps(string file, string flag)
{
    struct stat fileProps;
    stat(file.c_str(),&fileProps);
    string typeOfFile,rwPermissions,lmTime;
    auto stMode=fileProps.st_mode;

    if(S_ISDIR(stMode))
    {
        typeOfFile="d";
    }
    else if(S_ISSOCK(stMode))
    {
        typeOfFile="s";
    }
    else if(S_ISLNK(stMode))
    {
        typeOfFile="l";
    }
    else
    {
        typeOfFile="-";
    }

    rwPermissions="";
    //For user
    if((S_IRUSR & stMode))
    {
        rwPermissions+="r";
    }
    else
    {
        rwPermissions+="-";
    }

    if((S_IWUSR & stMode))
    {
        rwPermissions+="w";
    }
    else
    {
        rwPermissions+="-";
    }

    if((S_IXUSR & stMode))
    {
        rwPermissions+="x";
    }
    else
    {
        rwPermissions+="-";
    }

    //For group

    if((S_IRGRP & stMode))
    {
        rwPermissions+="r";
    }
    else
    {
        rwPermissions+="-";
    }

    if((S_IWGRP & stMode))
    {
        rwPermissions+="w";
    }
    else
    {
        rwPermissions+="-";
    }

    if((S_IXGRP & stMode))
    {
        rwPermissions+="x";
    }
    else
    {
        rwPermissions+="-";
    }

    //For others
    if((S_IROTH & stMode))
    {
        rwPermissions+="r";
    }
    else
    {
        rwPermissions+="-";
    }

    if((S_IWOTH & stMode))
    {
        rwPermissions+="w";
    }
    else
    {
        rwPermissions+="-";
    }

    if((S_IXOTH & stMode))
    {
        rwPermissions+="x";
    }
    else
    {
        rwPermissions+="-";
    }
    typeOfFile+=rwPermissions;

    long nOfLinks=fileProps.st_nlink;
    long size=fileProps.st_size;


    long ownerID=fileProps.st_uid;
    long groupID=fileProps.st_gid;
    struct passwd *pw;
    pw=getpwuid(ownerID);
    string ownerName=pw->pw_name;
    struct group *grp;
    grp=getgrgid(groupID);
    string grpName=grp->gr_name;

    auto mTime=fileProps.st_mtim;
    
    char buff[100];
    strftime(buff, 100, "%^b %d %H:%M", localtime(&mTime.tv_sec));

    if(flag.compare("def")==0)
    {
        string fileName=getFilename(file);
        if(fileName[0]=='.')
        {
            return;
        }
        printf("%s\n",fileName.data());
    }
    else if(flag.compare("-l")==0)
    {
        string fileName=getFilename(file);
        if(fileName[0]=='.')
        {
            return;
        }
        printf("%s %*ld %s %s %*ld %*s %s\n", typeOfFile.data(),3,nOfLinks,ownerName.data(),grpName.data(),7,size,15,buff,fileName.data());
    }
    else if(flag.compare("-a")==0)
    {
        string fileName=getFilename(file);
        printf("%s\n",fileName.data());
    }
    else 
    {
        string fileName=getFilename(file);
        printf("%s %*ld %s %s %*ld %*s %s\n", typeOfFile.data(),3,nOfLinks,ownerName.data(),grpName.data(),7,size,15,buff,fileName.data());
    }
    
}



void printLS0(string flag)
{
    char p[1000];
    char* nameOfDir=getcwd(p,1000);

    vector<string> filesList;
    DIR* dirp = opendir(nameOfDir);
    if(dirp==nullptr)
    {
        perror("Error while opening the directory");
        closedir(dirp);
        return;
    }
    struct dirent* dp;
    while ((dp = readdir(dirp)) != NULL)
    {
        filesList.push_back(dp->d_name);
    }
    closedir(dirp);

    for(int i=0;i<filesList.size();i++)
    {
        string dirNm=nameOfDir;
        string fileLoc=dirNm+"/"+filesList[i];
        printFileProps(fileLoc,flag);
    }

}


void printLS1(string nameOfDir,string flag)
{
    vector<string> filesList;
    DIR* dirp = opendir(nameOfDir.c_str());
    if(dirp==nullptr)
    {
        perror("An error occurred");
        closedir(dirp);
        return;
    }
    struct dirent* dp;
    while ((dp = readdir(dirp)) != NULL)
    {
        filesList.push_back(dp->d_name);
    }
    closedir(dirp);

    for(int i=0;i<filesList.size();i++)
    {
        string dirNm=nameOfDir;
        string fileLoc=dirNm+"/"+filesList[i];
        printFileProps(fileLoc,flag);
    }
}

void printLS2(vector<string> folders,string flag)
{
    int nOfFolders=folders.size();
    int i;
    for(i=0;i<nOfFolders-1;i++)
    {
        printf("%s:\n",folders[i].c_str());
        printLS1(folders[i],flag);
        printf("\n");
    }
    printf("%s:\n",folders[i].c_str());
    printLS1(folders[i],flag);
}

void handleLS(vector<string> arguments,string homePath)
{
    string defaultFlag="def";
    vector<string> folderList;
    vector<string> fileList;
    int length=arguments.size();
    
    for(int i=1;i<length;i++)
    {
        string arg=arguments[i];
        if(arg[0]=='-')
        {
            if(arg.compare("-l")==0)
            {
                if(defaultFlag.compare("-a")==0)
                {
                    defaultFlag="-al";
                }
                else
                {
                    defaultFlag="-l";
                }
            }
            else if(arg.compare("-a")==0)
            {
                if(defaultFlag.compare("-l")==0)
                {
                    defaultFlag="-al";
                }
                else
                {
                    defaultFlag="-a";
                }
            }
            else if((arg.compare("-al")==0)||(arg.compare("-la")==0))
            {
                defaultFlag="-al";
            }
            else
            {
                string errMsg="ls: Invalid flag "+arg+"\n";
                write(STDOUT_FILENO,errMsg.c_str(),errMsg.size());
                return;
            }
        }
        else
        {
            if(arg.compare("~")==0)
            {
                arg=homePath;
            }
            
            DIR* dirp = opendir(arg.c_str());
            if(dirp==nullptr)
            {
                if(errno==ENOTDIR)
                {
                    fileList.push_back(arg);
                }
                else
                {
                    folderList.push_back(arg);
                }
            }
            else
            {
                folderList.push_back(arg);
            }
            closedir(dirp);
            
        }
    }

    int nOfFolders=folderList.size();
    int nOfFiles=fileList.size();

    for(int i=0;i<nOfFiles;i++)
    {
        printFileProps(fileList[i],defaultFlag);   
    }

    if((nOfFiles>0)&&(nOfFolders>0))
    {
        printf("\n");
    }

    if((nOfFolders==0)&&(nOfFiles==0))
    {
        printLS0(defaultFlag);
    }
    else if(nOfFolders==1)
    {
        if(nOfFiles==0)
        {
            printLS1(folderList[0],defaultFlag);
        }
        else
        {
            printLS2(folderList,defaultFlag);
        }
    }
    else if(nOfFolders>=2)
    {
        printLS2(folderList,defaultFlag);
    }
}