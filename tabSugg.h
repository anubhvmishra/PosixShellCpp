#include<bits/stdc++.h>
#include<termios.h>
#include<sys/types.h>
#include<dirent.h>
#include<unistd.h> 


#include "cmdHist.h"


using namespace std;

string input="";
struct termios old_attributes,new_attributes;

int getTabSuggestion(string shellPrompt)
{
	
	bool status=true;
	
	char ch;
	vector<string> autoComplete({"ls","cd","pwd","cat","tac","echo","pinfo","search","history","exit","gedit","sleep","firefox","alias","apropos","apt","apt-get","aptitude","aspell","at","awk","basename","base32","base64","bash","bc","bg","bind","break","builtin","bzip2","cal","caller","case","cfdisk","chattr","chgrp","chmod","chown","chpasswd","chroot","chkconfig","cksum","clear","clear_console","cmp","comm","command","continue","cp","cpio","cron","crontab","csplit","curl","cut","date","dc","dd",
	"ddrescue","declare","df","diff","diff3","dig","dir","dircolors","dirname","dirs","dos2unix","dmesg","dpkg","du",
	"egrep", "eject", "enable", "env", "ethtool", "eval", "exec", "expand", "export", "expr",
	"false", "fdformat", "fdisk", "fg", "fgrep", "file", "find", "fmt", "fold", "for", "format", "free", "fsck", "ftp", "function", "fuser",
	"gawk", "getopts", "getfacl", "grep", "groupadd", "groupdel", "groupmod", "groups", "gzip", "hash", "head", "help", "hostname", "htop",
	"iconv", "id", "if", "ifconfig", "ifdown", "ifup", "import", "install", "iostat", "ip", "jobs", "join", "kill", "killall", "klist",
	"less", "let", "link", "ln", "local", "locate", "logname", "logout", "look", "lpc", "lpr", "lprint", "lprintd", "lprintq", "lprm", "lsattr", "lsblk", "ls", "lsof", "lspci",
	"make", "man", "mapfile", "md5sum", "mkdir", "mkfifo", "mkfile", "mkisofs", "mknod", "mktemp", "more", "most", "mount", "mtools", "mtr", "mv", "mmv",
	"nc", "netstat", "nft", "nice", "nl", "nohup", "notify-send", "nslookup", "open", "op",
	"passwd", "paste", "pathchk", "Perf", "ping", "pgrep", "pkill", "popd", "pr", "printcap", "printenv", "printf", "ps", "pushd", "pv",
	"quota", "quotacheck", "ram","rar","rcp","read","readarray","readonly","reboot","rename","renice","remsync","return","rev","rm", "rmdir", "rsync",
	"screen","scp","sdiff","sed","select","seq","set","setfacl","sftp","sha256sum","shift","shopt","shuf","shutdown","slocate","sort","source","split","ss","ssh","stat","strace","su","sudo","sum","suspend","sync",
	"tabs","tail","tar","tee","test","time","timeout","times","tmux","touch","top","tput","traceroute","trap","tr","true","tsort","tty","type",
	"ulimit","umask","umount","unalias","uname","unexpand","uniq","units","unix2dos","unrar","unset","unshar","until","uptime","useradd","userdel","usermod","users","uuencode","uudecode",
	"v","vdir","vi","vmstat","w","wait","watch","wc","whereis","which","while","who","whoami","wget","write",
	"xargs","xdg-open","xxd","xz","yes","zip","!!","#","."});
	vector<string> matchedWords;

	char wd[1000];
	auto name=getcwd(wd,1000);
	DIR* dirp = opendir(name);
    struct dirent* dp;
    while ((dp = readdir(dirp)) != NULL) {
        autoComplete.push_back(dp->d_name);
    }
    closedir(dirp);

	auto it = find(autoComplete.begin(), autoComplete.end(), ".");
	autoComplete.erase(it);

	it = find(autoComplete.begin(), autoComplete.end(), "..");
	autoComplete.erase(it);

	write(STDOUT_FILENO,shellPrompt.c_str(),shellPrompt.size());

	int indexOfCmd=0;
	while(status)
	{
		int readCh=read(STDIN_FILENO,&ch,1);
		if(ch==9)								// Tab Key
		{
			if(!input.empty())
			{
				int lastIndexOfWS=input.find_last_of(' ');
				int lastIndexOfSC=input.find_last_of(';');
				int toReadFrom=(lastIndexOfWS>lastIndexOfSC)?lastIndexOfWS:lastIndexOfSC;
				
				string searchString = input.substr(toReadFrom+1,(input.size()-toReadFrom));
				string stringUntilDelim = input.substr(0,toReadFrom+1);
				for(int i=0;i<autoComplete.size();i++)
				{
					if(autoComplete[i].find(searchString)==0)
					{
						matchedWords.push_back(autoComplete[i]);
					}
				}
				if(matchedWords.size()==1)
				{
					string toPrint="\r"+shellPrompt+stringUntilDelim+matchedWords[0];
					write(STDOUT_FILENO,toPrint.c_str(),toPrint.size());
					input=stringUntilDelim+matchedWords[0];
				}
				else if(matchedWords.size()>1)
				{
					write(STDOUT_FILENO,"\n",1);
					for(auto word:matchedWords)
					{
						string wordWs=word+' ';
						write(STDOUT_FILENO,wordWs.c_str(),wordWs.size());
					}
					string toPrint="\n"+shellPrompt+input;
					write(STDOUT_FILENO,toPrint.c_str(),toPrint.size());
				}
				matchedWords.clear();
			}
			indexOfCmd=0;
		}
		else if(ch==127)			// 127 is the delete character
		{
			if (!input.empty()) 
			{
		        input.pop_back(); // Remove the last character
		        write(STDOUT_FILENO,"\b \b",3); // Erase character on screen
            }
			indexOfCmd=0;
		}
		else if(ch==10)				// For enter key
		{
			setHistory(input);
			write(STDOUT_FILENO,"\n",1);
			indexOfCmd=0;
			return 0;
		}
		else if(ch=='\033')		// For up arrow
		{
			readCh=read(STDIN_FILENO,&ch,1); // skip [
			readCh=read(STDIN_FILENO,&ch,1); // getting 'A' for Up, B-down, C-right, D-left
			if((ch=='A'))
			{
				string hst = getHistory(indexOfCmd);
				if(hst!="")
				{
					string clearLine="\033[2K\033[0G";			 // clears the line and moves cursor to start of the line
					write(STDOUT_FILENO,clearLine.c_str(),clearLine.size()); // to remove extra characters in case pwd -> up arrow -> cd, should not show cdd
					string toPrint="\r"+shellPrompt+hst;
					write(STDOUT_FILENO,toPrint.c_str(),toPrint.size());
					indexOfCmd++;
					input=hst;
				}
			}
		}
		else if(ch==4)			//Ctrl+D
		{
			write(STDOUT_FILENO,"\n",1);
			return 1;
		}
		else
		{
			input+=ch;
			write(STDOUT_FILENO,&ch,1);
			indexOfCmd=0;
		}
		
	}
	return 0;
}