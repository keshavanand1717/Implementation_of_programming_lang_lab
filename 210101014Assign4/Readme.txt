SUBMISSION ASSIGNMENT 4 - CS 348
BY-
ANAND KESHAV
210101014

To compile the program run on the terminal the following commands - 
lex ans.l
yacc -d ans.y
cc lex.yy.c y.tab.c

To run the program run on the terminal the following commands - 
./a.out < input.pas
./a.out < input_error.pas
./a.out < input_error_2.pas


Important Points - 
1. A input files with the input pascal code must be in the same directory as the source code files.
2. I have provided one sample input files in the zip file.
3. ERRORS: I have handled the following semantic errors:
a. Duplicate declaration
b. Missing declaration
c. Missing initialization
d. Type Mismatch
4.lex.yy.c is generated after lex file compilation and y.tab.c and y.tab.h is generated after yacc file 
compilation. All of them are used to generate the final executable a.out.
5. I have written comments to explain the various parts of the code for better understanding of the program.