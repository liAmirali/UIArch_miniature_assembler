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
/**
 * Fills up the symbol table
 */
int fill_symtab(struct SymbolTable *, FILE *);
/**
 * Scans the assembly code entirely and writes the compiled code into the machine code file
 * @return 0 for succss and integers for any error
 */
int compile(FILE *assembly_file, FILE *machine_code_file);

// ?
int hex2int(char *);
void int2hex16(char *, int);
/**
 * Checks if a token is a valid instruction among the 15 defined instructions
 */
int is_instruction(char *);
/**
 * Removes the new line if it is the last character
 */
void remove_trailing_nline(char *);
/**
 * Returns an instruction struct with given tokens
 */
struct Instruction *compile_tokens(char *[4], size_t token_count);

/**
 * Parses the instruction and extracts the tokens
 * Returns the token count from the return value
 * and puts the actual tokens in the char array passed to the arguments
 */
size_t tokenize(char *, char **);
/**
 * Parses the fields seperated with commas
 */
size_t parse_fields_token(char *token, char **parsed);
/**
 * Checks if the field count matches with the instruction
 */
int get_number_of_fields(char *instruction);
/**
 * Initializes the console to print red
 */
void init_error();
/**
 * Resets console colors
 */
void reset_color();