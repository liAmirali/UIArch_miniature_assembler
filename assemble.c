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
    unsigned short int sp;

    // lines = (char **)malloc(15 * sizeof(char *));

    char *instructions[] = {"add", "sub", "slt", "or", "nand",
                            "addi", "slti", "ori", "lui", "lw", "sw", "beq", "jalr",
                            "j", "halt"};
    int instruction_count = 0;
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

    while (!feof(assem_file))
    {
        fgets(line, LINE_SIZE, assem_file);
        if (strcmp(line, "\n") != 0)
        {

            strcpy(lines[instruction_count], line);
            instruction_count++;
        }
    }

    sym_table = (struct SymbolTable *)malloc(sizeof(struct SymbolTable) * (instruction_count));

    fclose(assem_file);
    fclose(machine_file);
}
int findSymTabLen(FILE *inputFile)
{
    int count = 0;
    size_t line_size;
    char *line;
    line = (char *)malloc(line_size);
    while (getline(&line, &line_size, inputFile) != -1)
    {
        if ((line[0] == ' ') || (line[0] == '\t'))
            ;
        else
            count++;
    }
    rewind(inputFile);
    free(line);
    return count;
}
int fillSymTab(struct SymbolTable *symT, FILE *inputFile)
{
    int lineNo = 0;
    size_t line_size;
    char *line;
    int i = 0;
    char *token;
    line = (char *)malloc(line_size);
    while (getline(&line, &line_size, inputFile) != -1)
    {
        if ((line[0] == ' ') || (line[0] == '\t'))
            ;
        else
        {
            token = strtok(line, "\t, ");
            strcpy(symT[i].symbol, token);
            symT[i].value = lineNo;
            i++;
        }
        lineNo++;
    }
    rewind(inputFile);
    free(line);
    return lineNo;
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