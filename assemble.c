#include "assemble.h"

void main(int argc, char **argv)
{
    FILE *assem_file, *machine_file, *fopen();
    struct SymbolTable *sym_table;
    int sym_table_size;
    int i, j, found, noInsts;
    struct Instruction *curr_instruction;
    char *line;
    char *token;
    char *lines[TXT_SEG_SIZE];
    unsigned int sp;

    // lines = (char **)malloc(15 * sizeof(char *));

    char hex_table[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};

    i = 0;
    j = 0;
    line = (char *)malloc(LINE_SIZE);
    curr_instruction = (struct Instruction *)malloc(sizeof(struct Instruction));
    sp = 65536;

    if (argc < 3)
    {
        printf("***** Please run this program as follows:\n");
        printf("***** %s assprog.as machprog.m\n", argv[0]);
        printf("***** where assprog.as is your assembly program\n");
        printf("***** and machprog.m will be your machine code.\n");
        exit(1);
    }
    if ((assem_file = fopen(argv[1], "r")) == NULL)
    {
        printf("%s cannot be opened\n", argv[1]);
        exit(1);
    }
    if ((machine_file = fopen(argv[2], "w+")) == NULL)
    {
        printf("%s cannot be opened\n", argv[2]);
        exit(1);
    }

    sym_table = (struct SymbolTable *)malloc(sizeof(struct SymbolTable) * (TXT_SEG_SIZE));
    sym_table_size = fillSymTab(sym_table, assem_file);

    fclose(assem_file);
    fclose(machine_file);
}

int fillSymTab(struct SymbolTable *symT, FILE *inputFile)
{
    char *token;
    size_t lineSize = LINE_SIZE;
    char *line = (char *)malloc(lineSize * sizeof(char));
    int i = 0;
    char delimiter[4] = "\t ";
    int symTabLen = 0;

    while (getline(&line, &lineSize, inputFile) != -1)
    {
        removeTrailingNewLine(line);

        if (line == NULL || strcmp(line, "") == 0) continue;

        token = strtok(line, delimiter);

        if (token == NULL) continue;

        int isInst = isTokenAnInstruction(token);

        if (!isInst)
        {
            (symT + i)->symbol = malloc(strlen(token));
            strcpy((symT + i)->symbol, token);
            symT[i].value = i;
            symTabLen++;
        }
        i++;
    }
    rewind(inputFile);
    free(line);
    return symTabLen;
}

int hex2int(char *hex)
{
    int result = 0;
    while ((*hex) != '\0')
    {
        if (('0' <= (*hex)) && ((*hex) <= '9'))
            result = result * 16 + (*hex) - '0';
        else if (('a' <= (*hex)) && ((*hex) <= 'f'))
            result = result * 16 + (*hex) - 'a' + 10;
        else if (('A' <= (*hex)) && ((*hex) <= 'F'))
            result = result * 16 + (*hex) - 'A' + 10;
        hex++;
    }
    return (result);
}

void int2hex16(char *lower, int a)
{
    sprintf(lower, "%X", a);
    if (a < 0x10)
    {
        lower[4] = '\0';
        lower[3] = lower[0];
        lower[2] = '0';
        lower[1] = '0';
        lower[0] = '0';
    }
    else if (a < 0x100)
    {
        lower[4] = '\0';
        lower[3] = lower[1];
        lower[2] = lower[0];
        lower[1] = '0';
        lower[0] = '0';
    }
    else if (a < 0x1000)
    {
        lower[4] = '\0';
        lower[3] = lower[2];
        lower[2] = lower[1];
        lower[1] = lower[0];
        lower[0] = '0';
    }
}

int isTokenAnInstruction(char *str)
{
    if (str == NULL) return 0;

    for (int i = 0; i < INST_CNT; i++)
        if (strcmp(instructions[i], str) == 0) return 1;

    return 0;
}

void removeTrailingNewLine(char *str)
{
    int i = 0;
    while (str[i] != '\n')
    {
        if (str[i] == '\0') break;
        i++;
    }
    str[i] = '\0';
}