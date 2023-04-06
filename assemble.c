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
    sym_table_size = fill_symtab(sym_table, assem_file);

    fclose(assem_file);
    fclose(machine_file);
}

int fill_symtab(struct SymbolTable *symT, FILE *inputFile)
{
    char *token;
    size_t lineSize = LINE_SIZE;
    char *line = (char *)malloc(lineSize * sizeof(char));
    int i = 0;
    char delimiter[4] = "\t ";
    int symTabLen = 0;

    while (getline(&line, &lineSize, inputFile) != -1)
    {
        remove_trailing_nline(line);

        if (line == NULL || strcmp(line, "") == 0) continue;

        token = strtok(line, delimiter);

        if (token == NULL) continue;

        int isInst = is_token_inst(token);

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

char **tokenize(char *line)
{
    /**
     * label<white>instruction<white>field0,field1,field2<white>#comments
     * --^--       -----^-----       ---------^----------       ----^----
     *   1              2                     3                     4
     */

    size_t lineSize = LINE_SIZE;
    char *line = (char *)malloc(lineSize * sizeof(char));
    char delimiter[4] = "\t ";
    int tokenCount = 0;
    char *curr_token;
    char *tokens[4];
    int i = 0;

    curr_token = strtok(line, delimiter);
    while (curr_token != NULL)
    {
        tokenCount++;
        if (tokenCount == 4)
        {
            if (tokens[3][0] == '#')
            {
                // It's a comment from now on; so we just stop reading tokens here
                return tokens;
            }
            else
            {
                // If we read the 4th token and it didn't start with a '#', the line isn't in the correct format
                printf("Instruction is not in the following format:");
                printf("label<white>instruction<white>field0,field1,field2<white>#comments");
                return NULL;
            }
        }

        strcpy(tokens[i++], curr_token);

        printf("%s\n", curr_token);

        curr_token = strtok(NULL, delimiter);
    }

    return tokens;
}

struct Instruction compile_inst(char **tokens)
{
    char *token;
    size_t lineSize = LINE_SIZE;
    char *line = (char *)malloc(lineSize * sizeof(char));
    int i = 0;
    char delimiter[4] = "\t ";
    int tokenCount = 0;
    char *tokens[4];

    switch (tokenCount)
    {
    case 4:

        break;

    default:
        break;
    }
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

int is_token_inst(char *str)
{
    if (str == NULL) return 0;

    for (int i = 0; i < INST_CNT; i++)
        if (strcmp(instructions[i], str) == 0) return 1;

    return 0;
}

void remove_trailing_nline(char *str)
{
    if (str == NULL) return NULL;

    size_t size = strlen(str);
    if (str[size - 1] == '\n') str[size - 1] = '\0';
}

void print_error(char *err_msg)
{
}