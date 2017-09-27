# Tiger-Compiler (Undergrad Project)

Compiler for the <a href="http://www.cs.columbia.edu/~sedwards/classes/2002/w4115/tiger.pdf"> Tiger language</a>  defined in <a href="https://www.cs.princeton.edu/~appel/"> Andrew Appel</a>’s book <a href="https://www.cs.princeton.edu/~appel/modern/c/"> Modern Compiler Implementation in C</a> . Code generator generates code for a A Simple Target Machine <a href="http://www.cs.sjsu.edu/~louden/cmptext/contents.htm"> TM </a> 
defined in  <a href="http://www.cs.sjsu.edu/~louden/index.html"> Kenneth C. Louden</a>'s <a href="http://www.cs.sjsu.edu/~louden/cmptext/"> Compiler Construction </a> textbook.

 



  Authors: 
 
  (1)Gautam Singh 
  
  (2)Reuben D'souza



A given Tiger program is compiled to generate Three-Address-Code(TAC) which is then run on the simulator to get the desired output.
The compiler's code is in the folder named src. The simulator's code is in tm.c file. The output, TAC is generated in a file named TCODE.tm

To run in Windows platform, the code can be compiled in Turbo C to get executable file of ".exe" extension.The executable file "tm" can be run with this TAC file as the arguement. The "tm" has the following commands,


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


<span class='quora-content-embed' data-name='What-are-the-best-books-about-compilers-I-am-planning-to-take-a-graduate-compilers-class-and-I-have-some-background-knowledge-but-I-wanted-to-read-a-book-to-make-sure-Im-ready-I-know-the-Dragon-Book-is-popular-is-it-the-best/answer/Gautam-Singh'>Read <a class='quora-content-link' data-width='559' data-height='250' href='https://www.quora.com/What-are-the-best-books-about-compilers-I-am-planning-to-take-a-graduate-compilers-class-and-I-have-some-background-knowledge-but-I-wanted-to-read-a-book-to-make-sure-Im-ready-I-know-the-Dragon-Book-is-popular-is-it-the-best/answer/Gautam-Singh' data-type='answer' data-id='4179311' data-key='e60d55cae4786e3db9258d08fce8972a' load-full-answer='False' data-embed='diUJ33C'><a href='https://www.quora.com/Gautam-Singh'>Gautam Singh</a>&#039;s <a href='/What-are-the-best-books-about-compilers-I-am-planning-to-take-a-graduate-compilers-class-and-I-have-some-background-knowledge-but-I-wanted-to-read-a-book-to-make-sure-Im-ready-I-know-the-Dragon-Book-is-popular-is-it-the-best#ans4179311'>answer</a> to <a href='/What-are-the-best-books-about-compilers-I-am-planning-to-take-a-graduate-compilers-class-and-I-have-some-background-knowledge-but-I-wanted-to-read-a-book-to-make-sure-Im-ready-I-know-the-Dragon-Book-is-popular-is-it-the-best' ref='canonical'><span class="rendered_qtext">What are the best books about compilers? I am planning to take a graduate compilers class, and I have some background knowledge but I wanted to read a book to make sure I&#039;m ready. I know the Dragon Book is popular, is it the best?</span></a></a> on <a href='https://www.__nousername__.main.quora.com'>Quora</a><script type="text/javascript" src="https://www.quora.com/widgets/content"></script></span>



<span class='quora-content-embed' data-name='How-do-I-write-a-simple-compiler/answer/Gautam-Singh'>Read <a class='quora-content-link' data-width='559' data-height='250' href='https://www.quora.com/How-do-I-write-a-simple-compiler/answer/Gautam-Singh' data-type='answer' data-id='16308065' data-key='a0298ca4d6b93fa679b7a2f27d9fd5ab' load-full-answer='False' data-embed='diUJ33C'><a href='https://www.quora.com/Gautam-Singh'>Gautam Singh</a>&#039;s <a href='/How-do-I-write-a-simple-compiler#ans16308065'>answer</a> to <a href='/How-do-I-write-a-simple-compiler' ref='canonical'><span class="rendered_qtext">How do I write a simple compiler?</span></a></a> on <a href='https://www.__nousername__.main.quora.com'>Quora</a><script type="text/javascript" src="https://www.quora.com/widgets/content"></script></span>




