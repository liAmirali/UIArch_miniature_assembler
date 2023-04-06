#include "assemble.h"

void main(int argc, char **argv)
{
    FILE *assem_file, *machine_file, *fopen();
    struct SymbolTable *sym_table;
    int sym_table_size;
    int i, j, found;
    struct Instruction *curr_instruction;
    char *line;
    char *token;
    char *lines[TXT_SEG_SIZE];
    unsigned int stack_pointer;

    // lines = (char **)malloc(15 * sizeof(char *));

    char hex_table[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};

    i = 0;
    j = 0;
    line = (char *)malloc(LINE_SIZE);
    curr_instruction = (struct Instruction *)malloc(sizeof(struct Instruction));
    stack_pointer = 65536;

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
    int status = compile(assem_file, machine_file);

    if (status == 0)
        printf("Compiled successfully.");
    else
        printf("Compilation ended with an error.");

    fclose(assem_file);
    fclose(machine_file);
}

int compile(FILE *assembly_file, FILE *machine_code_file)
{
    size_t line_size = LINE_SIZE;
    char *line = (char *)malloc(line_size * sizeof(char));
    char *tokens[4];
    size_t token_count;

    while (getline(&line, &line_size, assembly_file))
    {
        remove_trailing_nline(line);

        if (line == NULL || strcmp(line, "") == 0) continue;

        token_count = tokenize(line, tokens);

        // ####TEMP CODE#####
        printf("token_count=%d", token_count);
        for (int i = 0; i < token_count; i++)
        {
            printf("tokens[%d]=%s", i, tokens[i]);
        }
        // ####TEMP CODE#####

        // compile_instruction(tokens, token_count);
    }
}

int fill_symtab(struct SymbolTable *symbol_table, FILE *inputFile)
{
    char *token;
    size_t line_size = LINE_SIZE;
    char *line = (char *)malloc(line_size * sizeof(char));
    int i = 0;
    char delimiter[4] = "\t ";
    int symbol_table_size = 0;

    while (getline(&line, &line_size, inputFile) != -1)
    {
        remove_trailing_nline(line);

        if (line == NULL || strcmp(line, "") == 0) continue;

        token = strtok(line, delimiter);

        if (token == NULL) continue;

        if (!is_instruction(token))
        {
            (symbol_table + i)->symbol = malloc(strlen(token));
            strcpy((symbol_table + i)->symbol, token);
            symbol_table[i].value = i;
            symbol_table_size++;
        }
        i++;
    }
    rewind(inputFile);
    free(line);
    return symbol_table_size;
}

size_t tokenize(char *line, char *tokens[4])
{
    /**
     * label<white>instruction<white>field0,field1,field2<white>#comments
     * --^--       -----^-----       ---------^----------       ----^----
     *   1              2                     3                     4
     */

    size_t line_size = LINE_SIZE;
    char delimiter[4] = "\t ";
    size_t token_count = 0;
    char *curr_token;
    int i = 0;

    curr_token = strtok(line, delimiter);
    while (curr_token != NULL)
    {
        token_count++;
        if (token_count == 4)
        {
            if (tokens[3][0] == '#')
            {
                // It's a comment from now on; so we just stop reading tokens here
                return token_count;
            }
            else
            {
                // If we read the 4th token and it didn't start with a '#', the line isn't in the correct format
                printf("Instruction is not in the following format:");
                printf("label<white>instruction<white>field0,field1,field2<white>#comments");
                return 0;
            }
        }

        strcpy(tokens[i++], curr_token);

        printf("%s\n", curr_token);

        curr_token = strtok(NULL, delimiter);
    }

    return token_count;
}

size_t parse_fields_token(char *token, char **parsed)
{
    char *fields[3];
    char *curr_field;
}

struct Instruction *compile_instruction(char *tokens[4], size_t token_count)
{
    char *instruction;
    char *fields_token;
    char *fields[3];
    char *field;
    size_t expected_field_count;
    size_t parsed_field_count;
    int i;

    struct Instruction *compiled_instruction = (struct Instruction *)malloc(sizeof(struct Instruction));

    return compiled_instruction;
}

// int check_valid_tokens(char *tokens[4], size_t token_count)
// {
//     // Parses the instruction and fields and checks the format
//     if (is_instruction(tokens[0]))
//     {
//         instruction = tokens[0];

//         expected_field_count = get_number_of_fields(instruction);

//         if (expected_field_count == 0 && tokens[1][0] != '#')
//         {
//             printf("Exptected no toke")
//         }

//         parsed_field_count = parse_fields_token(tokens[1], fields);

//         if (parse_fields_token != expected_field_count)
//         {
//             printf("Number of expected fields didn't match the expected amount.");
//             return NULL;
//         }
//     }
//     else if (is_instruction(tokens[1]))
//     {
//         instruction = tokens[1];
//         expected_field_count = get_number_of_fields(instruction);
//     }
//     else
//     {
//         printf("Invalid statement.");
//         return NULL;
//     }
// }

int get_number_of_fields(char *instruction)
{
    if (!is_instruction(instruction)) return -1;

    if (instruction == "lui" || instruction == "offset")
        return 2;
    else if (instruction == "offset")
        return 1;
    else if (instruction == "halt")
        return 0;
    else
        return 3;
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

int is_instruction(char *str)
{
    if (str == NULL) return 0;

    for (int i = 0; i < INST_CNT; i++)
        if (strcmp(instructions[i], str) == 0) return 1;

    return 0;
}

void remove_trailing_nline(char *str)
{
    if (str == NULL) return;

    size_t size = strlen(str);
    if (str[size - 1] == '\n') str[size - 1] = '\0';
}

void print_error(char *err_msg)
{
}