Alwien Dippenaar
V00849850
CSC 360 Assignment 1

Here are the instructions to succesfully run and test my code

1. Compile Code: Type "make PMan" 
	The make file will compile and create an excecutable for my program
2. Run Program: type "./PMan" 
	This will start executing the program.

Program supported commands

1. "bg ./{FILENAME}" 
	This command will start a process to run the specified {FILENAME} program in the background
	be sure to include "./" before the executable you wish to run with no space in between
2. "bglist"
	This command will print all the process currently runnning in the background
3. "bgkill {Process ID}"
	This command will send the SIGTERM signal to the specified {Process ID} to terminate the program
4. "bgstop {Process ID}"
	This command will send the SIGSTOP signal to the specified {Process ID} to stop executing the program
5. "bgstart {Process ID}"
	This command will send the SIGCONT signal to the specified {Process ID} to continue executing the program
6. "pstat {Process ID}"
	This command will print out the following information of the specified {Process ID} program:
		comm: The filename of the executing program in parentheses
		state: A character symbolizing the running state of the executing program
		utime: The amount of time the executing process has been scheduled in user mode
		stime: The amount of time the executing process has been scheduled in kernal mode
		rss: The Resident Set Size - The number of pages the executing program has in real memory
		voluntary_ctxt_switches: The number of voluntary context switches
		nonvoluntary_ctxt_switches: The number of nonvoluntary context switches  
