SUBMISSION ASSIGNMENT 2 - CS 348
BY-
ANAND KESHAV
210101014

To run the program run on the terminal the following commands - 
g++ -g assembler.cpp
./a.out input.txt
g++ -g linker_loader.cpp
./a.out output.txt

After running the 2-pass assembler code, object code can be found in output.txt file which is generated along
with listing.txt and intermediate.txt files.

After running the linker_loader file you will be prompted to enter the PROGADDR which the address where you 
want to load the program. So enter 0 (or other address where you want to load the program).
The memory address can be seen in the file memory.txt generated and line by line record can be found in 
line_by_line.txt file.

Important Points - 
1. A file named "input.txt" with the input assembly code must be in the same directory as the assembler.cpp source code.
2. I have provided one sample "input.txt" file which is the same as mentioned in the textbook.
3. FIELD SIZE is 10 for label and opcode and 30 for the operands. Extra column is provided for '+', '=' and '#'.
4. Comments have . symbol in the label field of the input file.
5. There should be no spaces before and after ',' while using indirect addressing.
6. I have written comments to explain the various parts of the code for better understanding of the program.
7. The program runs successfully and generates an empty file in case the input file is empty.