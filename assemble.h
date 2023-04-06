#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// the number of R, I and J type instructions
#define INST_CNT 15
#define NORTYPE 5
#define NOITYPE 8
#define NOJTYPE 2
#define RTYPE 0
#define ITYPE 1
#define JTYPE 2
// sizing constants
#define LINE_SIZE 72
#define TXT_SEG_SIZE 8192
/* this structure is defined to hold ever entity of symbol table    *
 * which, after finding the length, will become an array of symbols */
struct SymbolTable
{
    int value;
    char *symbol;
};

char *instructions[] = {"add", "sub", "slt", "or", "nand",
                        "addi", "slti", "ori", "lui", "lw", "sw", "beq", "jalr",
                        "j", "halt"};
/* every time an instruction is read, it will be tokenized and each   *
 * token will reside in one element of this structure               */
struct Instruction
{
    size_t instType; // 0 means r type, 1 means i type and 2 means j type
    size_t intInst;
    char *mnemonic;
    char inst[9];
    int rs;
    int rt;
    int rd;
    int imm;
    int PC;
};
/* the following function, findSymTabLen, will scan the assembly    *
 * code and count the number of symbols and return it              */
int findSymTabLen(FILE *);
/* this function fills up the symbol table                         */
int fillSymTab(struct SymbolTable *, FILE *);
/* and the following functions are other ancillary functions        */
void formInst(struct Instruction *, FILE *);
int hex2int(char *);
void int2hex16(char *, int);

int isTokenAnInstruction(char *);
void removeTrailingNewLine(char *);

/* parses the instruction and extracts the tokens */
struct Instruction tokenize(char *line);