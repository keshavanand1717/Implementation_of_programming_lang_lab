SUBMISSION ASSIGNMENT 3 - CS 348
BY-
ANAND KESHAV
210101014

To run the program run on the terminal the following commands - 
lex program.l
gcc lex.yy.c -ll (This is the command used in macOS for windows you may have to use "gcc -lfl lex.yy.c"
 command)
./a.out < input.pas

Important Points - 
1. A file named "input.pas" with the input pascal code must be in the same directory as the program.l source
   code.
2. I have written the comments to explain various parts of the code for better understanding of the program.
3. Running the "program.l" file generates a C file named "lex.yy.c" which is containing the code for the
   lexical analyzer based on the rules and patterns specified in the Lex file. 
4. You need to install the lex compiler to run the "program.l" file and then we need to compile the 
   "lex.yy.c" file as well which is then run on the input file.
5. The output is generated in the termial. 
6. I have printed the Symbol table as well. It columns consists of Bucket number, type, name and next pointer. 
7. Next pointer is given the value 0 if the bucket does not contain next symbol in the chain of the bucket.
   Also, I have not shown the buckets with no entries.