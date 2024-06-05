#include <iostream>
#include <sstream>
#include <cstdio>
#include <vector>
#include <string>
#include <iomanip>
#include <algorithm>
#include <map>

using namespace std;


// Structure to represent an assembly language instruction
struct instruction
{
    string label;
    string opcode;
    string operand;

    // Constructor to initialize the instruction
    instruction(string label = "", string opcode = "", string operand = "")
    {
        this->label = label;
        this->opcode = opcode;
        this->operand = operand;
    }

    // Check if the instruction is a comment
    bool is_comment()
    {
        return (label == ".");
    }

    // Check if the instruction has a label
    bool has_label()
    {
        return (label != "");
    }

    // Print the instruction for debugging
    void print()
    {
        cout << "-----------------------------------\n";
        cout << "label = " << label << '\n';
        cout << "opcode = " << opcode << '\n';
        cout << "operand = " << operand << '\n';
        cout << "-----------------------------------\n";
    }
};

// Function to convert opcode to corresponding machine code
string OPTAB(string opcode)
{
    // Mapping of opcodes to machine codes
    if (opcode == "LDA") return "00";
    else if (opcode == "JGT") return "34";
    else if (opcode == "JSUB") return "48";
    else if (opcode == "STX") return "10";
    else if (opcode == "STL") return "14";
    else if (opcode == "LDCH") return "50";
    else if (opcode == "STCH") return "54";
    else if (opcode == "ADD") return "18";
    else if (opcode == "WD") return "DC";
    else if (opcode == "LDX") return "04";
    else if (opcode == "LDL") return "08";
    else if (opcode == "STA") return "0C";
    else if (opcode == "RSUB") return "4C";
    else if (opcode == "TIX") return "2C";
    else if (opcode == "TD") return "E0";
    else if (opcode == "RD") return "D8";
    else if (opcode == "SUB") return "1C";
    else if (opcode == "MUL") return "20";
    else if (opcode == "DIV") return "24";
    else if (opcode == "COMP") return "28";
    else if (opcode == "J") return "3C";
    else if (opcode == "JLT") return "38";
    else if (opcode == "JEQ") return "30";
    return "";
}

// Symbol table to store labels and their corresponding addresses
map<string, int> SYMTAB;


// Function to process a line in pass 1
void process_line(string line, instruction *instr)
{
    string label, opcode, operand;
    vector<string> vec;
    int temp  = 0;
    
    // Breaking up the line and storing in a vector
    istringstream iss(line);
    string token;
    while (iss >> token) {
        vec.push_back(token);
    }

    // Check if the line is a comment
    if (vec[0] == ".")
        instr->label = vec[0];
    else if (vec.size()==1)
    {
        instr->opcode = vec[0];
    }
    else if(vec.size()==2){
        instr->opcode = vec[0];
        instr->operand = vec[1];
    }
    else
    {
        instr->label = vec[0];
        instr->opcode = vec[1];
        instr->operand = vec[2];
    }
}

// Function to process a line in pass 2
string process_line_2(string line, instruction *instr)
{
    string label, opcode, operand,loc;
    vector<string> newVec;
    int temp  = 0;
    
    istringstream iss(line);
    string token;
    while (iss >> token) {
        newVec.push_back(token);
    }

    if (newVec[0] == ".")
    {
        label = newVec[0];
        instr->label = label;
        return "";
    }
    else if (newVec.size()==2)
    {
        loc = newVec[0];
        instr->opcode = newVec[1];
        return loc;
    }
    else if (newVec.size()==3)
    {
        loc = newVec[0];
        instr->opcode = newVec[1];
        instr->operand = newVec[2];
        return loc;
    }
    else{
        loc = newVec[0];
        instr->label = newVec[1];
        instr->opcode = newVec[2];
        instr->operand = newVec[3];
        return loc;
    }
}

// Function to format a number with a specified width and optionally in hex
string num_format(string input, int width, bool hex)
{
    int num;
    if (hex)
        num = stoi(input, NULL, 16);
    else
        num = stoi(input);

    char buffer[20];
    snprintf(buffer, sizeof(buffer), "%0*X", width, num);
    return string(buffer);
}

// Function to format a name with a specified width
string name_format(string s, int width)
{
    char queue[20];
    snprintf(queue, sizeof(queue), "%-*s", width, s.c_str());
    return string(queue);
}

// Pass 1 of the assembler
int pass_1()
{
    FILE *pointerIn, *pointerOut;

    pointerIn = fopen("input.txt", "r");
    pointerOut = fopen("intermediate.txt", "w");

    // Variables for location counter, start address, and program length
    int loc = 0, STADDR = 0, LENGTH;

    bool error = false; // Flag to indicate any errors during processing

    // Process each line in the input file
    char line[256]; // Buffer to store each line read from the file
    while (fgets(line, sizeof(line), pointerIn))
    {
        instruction instr;
        // Process the current line and populate the instruction structure
        process_line(line, &instr);

        if (!instr.is_comment())
        {
            if (instr.opcode == "START")
            {
                STADDR = stoi(instr.operand, NULL, 16);
                loc = STADDR;
                fprintf(pointerOut, "%-10s%s", num_format(to_string(loc), 4, false).c_str(), line);
            }
            else if (instr.opcode == "END")
            {
                LENGTH = loc - STADDR;
                fprintf(pointerOut, "%-10s%s", num_format(to_string(loc), 4, false).c_str(), line);
                break;    // Exit the loop when END is encountered
            }
            else
            {
                if (instr.has_label())
                {
                    // Update the symbol table with the label and its address
                    if (SYMTAB.find(instr.label) == SYMTAB.end())
                        SYMTAB[instr.label] = loc;
                    else
                    {
                        error = true;
                        cout << "duplicate symbol\n";
                    }
                }

                fprintf(pointerOut, "%-10s%s", num_format(to_string(loc), 4, false).c_str(), line);
                
                // Update location counter based on the opcode
                if (OPTAB(instr.opcode) != "")
                    loc += 3;
                else if (instr.opcode == "WORD")
                    loc += 3;
                else if (instr.opcode == "RESW")
                    loc += 3 * stoi(instr.operand);
                else if (instr.opcode == "RESB")
                    loc += stoi(instr.operand);
                else if (instr.opcode == "BYTE")
                {
                    if (instr.operand[0] == 'C')
                        loc += (instr.operand.length() - 3);
                    if (instr.operand[0] == 'X')
                        loc += (instr.operand.length() - 3) / 2;
                }
                else
                {
                    error = true;
                    cout << "invalid operation code1\n";
                }
            }
        }
        else
            fprintf(pointerOut, "%s", line); // Write comments to the output file
    }

    // file pointers closed
    fclose(pointerIn);
    fclose(pointerOut);

    return LENGTH;
}

// Pass 2 of the assembler
void pass_2(int LENGTH)
{
    FILE *pointerIn, *pointerOut, *pointerOut2; // file pointers intialised

    pointerIn = fopen("intermediate.txt", "r");
    pointerOut = fopen("output.txt", "w");
    pointerOut2 = fopen("listing.txt", "w");

    string text_record, staddr; // Variables for storing text records and start address
    bool error = false; // Flag to indicate any errors during processing

    char line[256]; // Buffer to store each line read from the file
    while (fgets(line, sizeof(line), pointerIn))
    {
        size_t len = strlen(line);
        if (len > 0 && line[len - 1] == '\n') {
            line[len - 1] = '\0';  // Remove the newline character
        }
        instruction instr;
        string loc = process_line_2(line, &instr);

        if (!instr.is_comment())
        {
            if (instr.opcode == "START")
            {
                // Write header record for the start instruction
                fprintf(pointerOut, "H%-6s%s%s%s", instr.label.c_str(), num_format(instr.operand, 6, true).c_str(), num_format(to_string(LENGTH), 6, false).c_str(), "\n");
                fprintf(pointerOut2, "%s\n", line);
            }
            else if (instr.opcode == "END")
            {
                // Write text record for the end instruction
                if (text_record.length())
                {
                    int length = (text_record.length()) / 2;
                    fprintf(pointerOut, "T%s%s%s\n", num_format(staddr, 6, true).c_str(), num_format(to_string(length), 2, false).c_str(), text_record.c_str());
                }
                fprintf(pointerOut2, "%s", line);

                // Write end record
                int first = SYMTAB[instr.operand];
                fprintf(pointerOut, "E%s\n", num_format(to_string(first), 6, false).c_str());
            }
            else
            {
                string obcode = "";
                if (OPTAB(instr.opcode) != "")
                {
                    obcode = OPTAB(instr.opcode);
                    if (instr.operand != "")
                    {
                        int operand = 0;
                        if (instr.operand.find(",") != string::npos)
                        {
                            instr.operand.resize(instr.operand.find(","));
                            operand += 32768;
                        }
                        if (SYMTAB.find(instr.operand) != SYMTAB.end())
                        {
                            operand += SYMTAB[instr.operand];
                            obcode += num_format(to_string(operand), 4, false);
                        }
                        else
                        {
                            obcode += num_format("0", 4, false);
                            error = true;
                            cout << "undefined symbol\n";
                        }
                    }
                    else
                    {
                        obcode += num_format("0", 4, false);
                    }
                    
                    // Write line with formatted object code to listing file
                    fprintf(pointerOut2, "%s%s\n", line, name_format(obcode, 10).c_str());
                }
                else
                {
                    if (instr.opcode == "BYTE")
                    {
                        if (instr.operand[0] == 'C')
                        {
                            // Process BYTE instruction with character constant
                            string constant = instr.operand.substr(2, instr.operand.length() - 3);
                            for (size_t i = 0; i < constant.length(); ++i)
                            {
                                char ch = constant[i];
                                int x = ch;
                                obcode += num_format(to_string(x), 2, false);
                            }

                        }
                        else
                        {
                            // Process BYTE instruction with hexadecimal constant
                            string constant = instr.operand.substr(2, instr.operand.length() - 3);
                            obcode += num_format(constant, constant.length(), true);
                        }
                    }
                    else if (instr.opcode == "WORD")
                        obcode += num_format(instr.operand, 6, false);

                    // Write line with formatted object code to listing file
                    fprintf(pointerOut2, "%s%s\n", line, name_format(obcode, 10).c_str());
                }

                // Update text record based on the generated object code
                if (text_record.length() == 0)
                {
                    staddr = loc;
                }

                if (text_record.length() + obcode.length() <= 60 && instr.opcode != "RESW" && instr.opcode != "RESB")
                {
                    text_record += obcode;
                }
                else
                {
                    if (text_record.length())
                    {
                        int length = (text_record.length()) / 2;
                        // Write text record to output file
                        fprintf(pointerOut, "T%s%s%s\n", num_format(staddr, 6, true).c_str(), num_format(to_string(length), 2, false).c_str(), text_record.c_str());
                        text_record = obcode;
                        staddr = loc;
                    }
                }
            }
        }
        else
            fprintf(pointerOut2, "%s\n", line);
    }

    // Close the files
    fclose(pointerIn);
    fclose(pointerOut);
    fclose(pointerOut2);
}

int main()
{
    int LENGTH = pass_1();
    pass_2(LENGTH);
    return 0;
}
