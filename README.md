# Tiger-Compiler (Undergrad Project)

Compiler for the <a href="http://www.cs.columbia.edu/~sedwards/classes/2002/w4115/tiger.pdf"> Tiger language</a>  defined in <a href="https://www.cs.princeton.edu/~appel/"> Andrew Appel</a>’s book <a href="https://www.cs.princeton.edu/~appel/modern/c/"> Modern Compiler Implementation in C</a> . Code generator generates code for a A Simple Target Machine <a href="http://www.cs.sjsu.edu/~louden/cmptext/contents.htm"> TM </a> 
defined in  <a href="http://www.cs.sjsu.edu/~louden/index.html"> Kenneth C. Louden</a>'s <a href="http://www.cs.sjsu.edu/~louden/cmptext/"> Compiler Construction </a> textbook.

 



  Authors: 
 
  (1)Gautam Singh 
  
  (2)Reuben D'souza



A given Tiger program is compiled to generate Three-Address-Code(TAC) which is then run on the simulator to get the desired output.
The compiler's code is in the folder named src.
The simulator's code is in tm.c file.

The output, TAC is generated in a file named TCODE.tm

To run in Windows platform, the code can be compiled in Turbo C to get executable file of ".exe" extension.
The executable file "tm" can be run with this TAC file as the arguement.
The "tm" has the following commands,


1.step <n>             		: Execute n (default 1) TM instructions

2.go                      	: Execute TM instructions until HALT

3.regs                    	: Print the contents of the registers.

4.iMem <b><n>    		: Print n iMem locations starting at b

5.dMem <b><n>  	        	: Print n dMem locations starting at b

6.trace				: Toggle instructions trace.

7.print				: Toggle print of total instructions executed.

8.clear				: Reset simulator for new execution of program.

9.help 				: Cause this list of commands to be printed.

10.quit				: Terminate the simulation.




