AUX: SPACE
N: SPACE 
ONE: CONST 1
INPUT	N
LOAD N
FAT:         SUB 						ONE
JMPZ 				FIM
STORE 			AUX
MULT                                          N
STORE N
LOAD AUX 
JMP FAT
FIM: OUTPUT N
STOP


