# P0VM
A 32-bit Virtual Machine for the P0VM Instruction Set.

Compiler Construction Project 2020 fwaldner@kth.se

See source-code and report for details

## Building 
The project is defined as a Visual Studio Solution file. Building the VM has only been tested on Windows 10.

However, it should be straightforward to compile the source on other platforms and without any specific build systems.

## Running the VM
Supply input program as command line argument:

P0VM.exe program.pvm

If no command line argument is given, the program default to look for a file called program.pvm

## Test programs
The folder Program contains test programs that can be executed by the VM
