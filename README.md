# Operating-System

  Created a basic Operating System for Windows using C consisting of: 
  - kernel: core of the OS, contains the scheduler
  - shell: 
  - shellmemory: stores variables used by the OS
  - interpreter: interprets the commands and calls the corresponding functions
  - parser: parses the commands sent by the user 
  - pcb: process control block containing information about a specific process
  - cpu: simulates the CPU of a computer
  - ram: simulates the memory of a computer, 10 frames of 4 lines of code each
  - memorymanager: responsible for loading the instructions of a program into RAM
  
  
  For a complete list of the shell commands, type help and press enter into the shell's command line.
  
  The user can either manually type instructions or run text files as scripts using the exec command.
  When using the exec command, the OS first goes through the launching procedure for each script. It checks if the file can be opened. It then copies the file into the Backing Store folder and loads its first 2 pages into RAM. A page represents 4 lines of code. Afterwards, it creates a pcb struct and adds the pcb to the Ready Queue of pcbs. 
  
  Once all the launched scripts have finished their launching procedure, the OS starts running the programs. Each pcb has access to the CPU for a quanta before it must give it up to the next pcb. A quanta consists of 2 lines of code. When a pcb has finished executing, it is removed from the Ready Queue, the RAM it used is cleared, and the corresponding file in the Backing Store is deleted.
  
  Since this OS uses paging, it must be able to handle page faults. Whenever the pcb is trying to run a line of code that isn't in the RAM, a page fault occurs. The memorymanager checks if a free frame is available. If a free frame is available, the following page of instructions is loaded into the frame, otherwise a victim frame is randomly selected.
