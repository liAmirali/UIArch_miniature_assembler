#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// the number of R, I and J type instructions and directives
#define INST_CNT 15
#define DRCT_CNT 2
#define NORTYPE 5
#define NOITYPE 8
#define NOJTYPE 2
#define RTYPE 0
#define ITYPE 1
#define JTYPE 2
// sizing constants
#define MAX_LABEL_LEN 6
#define LINE_SIZE 72
#define TXT_SEG_SIZE 8192
#define MIN_SGN_OFFSET -32768
#define MAX_SGN_OFFSET 32767
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
char *directives[] = {".fill", ".space"};
/* every time an instruction is read, it will be tokenized and each   *
 * token will reside in one element of this structure               */
struct Instruction
{
    size_t instType; // 0 means r type, 1 means i type and 2 means j type
    char inst[9];
    int rs;
    int rt;
    int rd;
    int imm;
};

/**
 * Fills up the symbol table
 */
size_t fill_symtab(struct SymbolTable *, FILE *);

/**
 * Scans the assembly code entirely and writes the compiled code into the machine code file
 * @return 0 for success or other integers for any error
 */
int compile(FILE *assembly_file, FILE *machine_code_file, struct SymbolTable *, size_t, int txt_seg[TXT_SEG_SIZE]);

/**
 * Searches through the symbol table and returns the value inside it
 */
int get_label_value(struct SymbolTable *, size_t, char *);

/**
 * Searches through the symbol table and checks if the label exists or not
 */
int label_exists(struct SymbolTable *, size_t, char *);

/**
 * Parses the instruction and extracts the tokens
 * Returns the token count from the return value
 * and puts the actual tokens in the char array passed to the arguments
 */
size_t tokenize(char *, char **);

/**
 * Returns an instruction struct with given tokens
 */
struct Instruction *form_instruction(char *instruction, char **fields, int, struct SymbolTable *symbol_table, size_t symbol_table_size);

/**
 * Returns the hex equivalent of the instruction
 */
void get_instruction_hex(struct Instruction *, char[9]);

/**
 * Checks the correctness of tokens
 * 1- The instruction has to be a valid existing instruction
 * 2- The fields count must match the instruction mnemonic
 */
int check_valid_tokens(char **tokens, size_t token_count, char *instruction, char **fields);

/**
 * Parses the fields separated with commas
 */
size_t parse_fields_token(char *token, char **parsed);

/**
 * Checks if the field count matches with the instruction
 */
int get_number_of_fields(char *instruction);

/**
 * Returns the number of instruction type
 */
size_t get_instruction_type(char *instruction);

/**
 * Returns the opcode of the instruction
 */
int get_instruction_opcode(char *instruction);

// ?
int hex2int(char *);
void int2hex16(char *, int);

/**
 * Returns hex equivalent of an int between 0-15
 */
char get_hex_digit(int n);

/**
 * Checks if a token is a valid instruction among the 15 defined instructions
 */
int is_instruction(char *);

/**
 * Checks if a token is a valid directive among the 2 defined directives
 */
int is_directive(char *);

/**
 * Checks if the string is a valid label name
 */
int is_label_name_valid(char *);

/**
 * Checks if the string is contains only digits
 */
int is_numeric(char *);

/**
 * Removes the new line if it is the last character
 */
void remove_trailing_nline(char *);

/**
 * Initializes the console to print red
 */
void init_error();

/**
 * Resets console colors
 */
void reset_color();