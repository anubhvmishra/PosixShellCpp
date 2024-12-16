# POSIX Shell Implementation
- Implementation of a POSIX shell in CPP that supports semi-colon separated list of commands.
- Home directory is the one where executable files are stored.

## Commands Supported

### cd
- cd, cd ~ : Takes us to the home directory.
- cd . : Remains in the current directory.
- cd .. : Takes us to the parent directory.
- cd - : Takes us to the previous opened directory(maybe child or parent, whichever was opened last).
- cd foldername : Takes us inside the foldername directory.

### echo
- echo "string" : Prints the string as it is from the first occurrence of (") till the last occurrence of ("). Rest of the input is ignored.
- An error message is generated if the argument contains less than 2 double quotes.

### ls
- ls, ls. : Prints list of files/folders(excluding hidden ones) inside current directory.
- ls -a : Prints list of all the files/folders(including hidden ones) inside current directory.
- ls -l : Prints verbose list of files/folders(excluding hidden ones) inside current directory.
- ls .. : Prints list of files/folders(excluding hidden ones) inside parent directory.
- ls ~ : Prints list of files/folders(excluding hidden ones) inside our home directory.
- ls -a -l, ls -al, ls -la : Verbose list of all the files/folders(including hidden ones).
- ls Directory/File_name : Prints list of files/folders(excluding hidden ones) inside Directory. For File_name, prints the name of the file.
- ls -flags Directory/File_name : Prints list of files/folders as specified by the flags inside Directory. For File_name, prints the entire details of the file in case of -l flag.

### &
- Makes the process specified by the command run in background and prints its process ID.
- Multiple background processes can be created.

### pinfo
- pinfo: Prints the process-related info of our shell process.
- pinfo pid: Prints the process-related info of the process specified by pid.

### search
- search file/folder : Searched for a file/folder. Prints True, if found, else prints False.
- Search is currently supported upto one level inside the current directory.

### I/O redirection
- Command_1 < filename : Command_1 takes its input from filename instead of stdin.
- Command_2 > filename : Command_2 prints its output to filename instead of stdout.
- A combination of input and output redirection is supported.
- Upto one redirection(input/output/input+output) is currently supported.

### Pipeline
- Command_1 | Command_2 : Redirects the output of the Command_1 as input to the Command_2.
- Multiple pipelines along with redirection is supported.

### Signals
- CTRL+C : Kills the currently running foreground process. It has no effect on the shell.
- CTRL+Z : Pushes currently running foreground process into the background and stops its execution. Difference from CTRL+C is that it does not terminate the process, the process can be continued again by sending proper signals.
- CTRL+D : Logs us out of the shell.

### Autocomplete(TAB) key
- Pressing TAB key autocompletes any file/folder names inside current directory.
- It can also be used to autocomplete commands.
- In case of more than one matching, prints all of them in a newline.

### History
- history : Prints last 10 commands executed by the shell.
- history n : Prints last n commands executed by the shell(n should be a positive integer).
- Upto last 20 commands are stored currently. Any value of n greater than 20 will print last 20 commands.
- History is retained across all the sessions of the shell.

### UP Arrow Key
- UP Arrow Key will loop over the previous commands present in our shell’s history and show them on the prompt until it reaches the last command.
- Other Arrow Keys (Left, Right, Down) are currently not supported.

## Installation

Install any C++ compiler to run the following codes. In linux we can do

```bash
sudo apt install g++
```

## Usage
Run make

```bash
make
```
Run ./s

```bash
./s
```

## Contributing

Pull requests are welcome. For major changes, please open an issue first
to discuss what you would like to change.

Please make sure to update tests as appropriate.

