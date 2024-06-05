%{
    /* Include necessary header files */
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>

    /* Definitions of manifest constants for tokens */
    #define PROGRAM_TOKEN   1
    #define VAR_TOKEN       2
    #define BEGIN_TOKEN     3
    #define END_TOKEN       4
    #define END_DOT_TOKEN   5
    #define INTEGER_TOKEN   6
    #define FOR_TOKEN       7
    #define READ_TOKEN      8
    #define WRITE_TOKEN     9
    #define TO_TOKEN        10
    #define DO_TOKEN        11
    #define SEMICOLON_TOKEN 12
    #define COLON_TOKEN     13
    #define COMMA_TOKEN     14
    #define ASSIGN_TOKEN    15
    #define ADD_TOKEN       16
    #define SUB_TOKEN       17
    #define MUL_TOKEN       18
    #define DIV_TOKEN       19
    #define OPEN_BRACE_TOKEN    20
    #define CLOSE_BRACE_TOKEN   21
    #define ID_TOKEN        22
    #define INT_TOKEN       23

    /* Hashtable implementation */
    #define SIZE 20

    typedef struct Symbol {
        char* name;
        char type;
        struct Symbol* next;
    } Symbol;

    Symbol* symbolTable[SIZE];

    int hashFunction(char* name) {
        int len = strlen(name);
        // Simple hash function
        int hash = 0;
        for(int i = 0; i < len; i++) 
            hash += (int) name[i];
        return hash % SIZE;
    }

    void initializeSymbolTable() {
        // Initialize all buckets to NULL
        for(int i = 0; i < SIZE; i++) 
            symbolTable[i] = NULL;
    }
   int hashfunction(char* name) {
     unsigned long hash = 5381;
     int c;
     while ((c = *name++)) {
          hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
     }
     return hash % SIZE;
   }

    Symbol* searchSymbol(char* name) {
        // Compute the hash function for bucket index
        int index = hashFunction(name);

        // Search the linked list associated with the bucket
        Symbol* temp = symbolTable[index];
        while(temp != NULL && strcmp(temp->name, name) != 0)
            temp = temp->next;

        return temp;
    }

    Symbol* insertSymbol(char* name, char type) {
        // Compute the hash function for bucket index
        int index = hashFunction(name);

        // Create the new symbol
        Symbol* newSymbol = (Symbol*)malloc(sizeof(Symbol));
        newSymbol->name = strdup(name);
        newSymbol->type = type;

        // Insert it at the beginning of the bucket linked list
        newSymbol->next = symbolTable[index];
        symbolTable[index] = newSymbol;

        return newSymbol;
    }

    void printSymbolTable() {
     // Pretty printing of symbol table
     printf("**********************************************************\n");
     printf("Symbol Table\n");
     printf("---------------------------------------------------------\n");
     printf("| Bucket |    Type    |    Name    |    Next Pointer    |\n");
     printf("---------------------------------------------------------\n");
     for(int i = 0; i < SIZE; i++) {
          Symbol* temp = symbolTable[i];
          while(temp != NULL) {
               printf("| %6d | %10c | %10s | %18p |\n", i, temp->type, temp->name, temp->next);
               temp = temp->next;
          }
     }
     printf("---------------------------------------------------------\n");
     printf("**********************************************************\n");
     }


    /* Declaration of useful functions and variables */
    void* yylval;
    void* installIdentifier();
    void* installNumber();
    int lineNumber = 0;
%}

/* Define regular expressions */
delim       [ \t\n]
letter      [A-Za-z]
digit       [0-9]
id          {letter}({letter}|{digit})*
integer     {digit}+

%%

\n          { lineNumber++; }
[ \t]       { }
PROGRAM     { return PROGRAM_TOKEN; }
VAR         { return VAR_TOKEN; }
BEGIN       { return BEGIN_TOKEN; }
END         { return END_TOKEN; }
END\.       { return END_DOT_TOKEN; }
INTEGER     { return INTEGER_TOKEN; }
FOR         { return FOR_TOKEN; }
READ        { return READ_TOKEN; }
WRITE       { return WRITE_TOKEN; }
TO          { return TO_TOKEN; }
DO          { return DO_TOKEN; }
\;          { return SEMICOLON_TOKEN; }
\:          { return COLON_TOKEN; }
\,          { return COMMA_TOKEN; }
\:\=        { return ASSIGN_TOKEN; }
\+          { return ADD_TOKEN; }
\-          { return SUB_TOKEN; }
\*          { return MUL_TOKEN; }
DIV         { return DIV_TOKEN; }
\(          { return OPEN_BRACE_TOKEN; }
\)          { return CLOSE_BRACE_TOKEN; }
{id}        { yylval = (void*)installIdentifier(); return ID_TOKEN; }
{integer}   { yylval = (void*)installNumber(); return INT_TOKEN; }
. {
    fprintf(stderr, "Error: Unexpected token found.\n");
    exit(EXIT_FAILURE);
}

%%

void* installIdentifier() {
    Symbol* sym = searchSymbol(yytext);
    if(sym == NULL)
        sym = insertSymbol(yytext, '^'); // Symbol for identifier
    return sym;
}

void* installNumber() {
    Symbol* sym = searchSymbol(yytext);
    if(sym == NULL)
        sym = insertSymbol(yytext, '#'); // Symbol for number
    return sym;
}

int main() {
    initializeSymbolTable();

    int token;
    int prevLineNumber = 0;

    /* Parsing every token of the input program */
    token = yylex();
    while(token) {

        /* Printing the line number */
        if(lineNumber == 0) {
            lineNumber++;
            printf("%10d", lineNumber);
        } else {
            if(lineNumber == prevLineNumber)
                printf("%10c", ' ');
            else
                printf("%10d", lineNumber);
        }

        /* Printing the token ID, type, and specifier */
        Symbol* sym = NULL;
        if(token == ID_TOKEN || token == INT_TOKEN)
            sym = (Symbol*)yylval;
        if(sym != NULL)
            printf("%10d %10c%s\n", token, sym->type, sym->name);
        else
            printf("%10d \n", token);

        prevLineNumber = lineNumber;
        token = yylex();
    }

    printSymbolTable();
    return 0;
}
