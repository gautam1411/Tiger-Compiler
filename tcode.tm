* Tiger Compilation to TM Code
* File: tcode
  0:     LD  6,0(0) 	load maxaddress from location 0
  1:    LDC  2,3000(0) 	stack....
  2:     ST  0,0(0) 	clear location 0
* -> Const
  4:    LDC  0,5(0) 	load const
* <- Const
  5:     ST  0,0(6) 	op: push left
* -> Const
  6:    LDC  0,6(0) 	load const
* <- Const
  7:     ST  0,-1(6) 	op: push left
* -> Const
  8:    LDC  0,0(0) 	load const
* <- Const
  9:     ST  0,-2(6) 	op: push left
* -> Id
 10:     LD  0,0(6) 	load id value
* <- Id
 11:     IN  1,0,0 	read integer value
 12:     ST  1,0(6) 	read: store value
* -> Id
 13:     LD  0,-1(6) 	load id value
* <- Id
 14:     IN  1,0,0 	read integer value
 15:     ST  1,-1(6) 	read: store value
* -> if
* -> Op
* -> Id
 16:     LD  0,0(6) 	load id value
* <- Id
 17:     ST  0,-3(6) 	op: push left
* -> Id
 18:     LD  0,-1(6) 	load id value
* <- Id
 19:     LD  1,-3(6) 	op: load left
 20:    SUB  0,1,0 	op <
 21:    JLT  0,2(7) 	br if true
 22:    LDC  0,0(0) 	false case
 23:    LDA  7,1(7) 	unconditional jmp
 24:    LDC  0,1(0) 	true case
* <- Op
* if: jump to else belongs here
* -> Id
 26:     LD  0,-1(6) 	load id value
* <- Id
 27:    OUT  0,0,0 	write ac
* if: jump to end belongs here
 25:    JEQ  0,3(7) 	if: jmp to else
* -> Id
 29:     LD  0,0(6) 	load id value
* <- Id
 30:    OUT  0,0,0 	write ac
 28:    LDA  7,2(7) 	jmp to end
* <- if
  3:    LDA  7,0(7) 	jump to fn
* End of execution.
 31:   HALT  0,0,0 	
