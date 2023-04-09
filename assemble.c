#include "assemble.h"

void main(int argc, char **argv)
{
    FILE *assem_file, *machine_file, *fopen();
    struct SymbolTable *sym_table;
    int text_data_seg[TXT_SEG_SIZE];
    size_t sym_table_size;

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
    int status = compile(assem_file, machine_file, sym_table, sym_table_size, text_data_seg);

    if (status == 0)
        printf("Compiled successfully.\n");
    else
        printf("Compilation ended with errors.\n");

    free(sym_table);
    fclose(assem_file);
    fclose(machine_file);
}

int compile(FILE *assembly_file, FILE *machine_code_file, struct SymbolTable *symbol_table, size_t symbol_table_size, int txt_seg[TXT_SEG_SIZE])
{
    size_t line_size = LINE_SIZE;
    char *line = (char *)malloc(line_size * sizeof(char));
    char **tokens;
    size_t token_count;
    int are_tokens_valid;
    int data_seg_top = 0;

    struct Instruction *instruction = (struct Instruction *)malloc(sizeof(struct Instruction));

    char *detected_instruction;
    char **detected_fields;

    char instruction_hex[9];

    tokens = (char **)malloc(4 * sizeof(char *));
    for (int i = 0; i < 4; i++)
        tokens[i] = (char *)malloc(16 * sizeof(char));

    detected_instruction = (char *)malloc(5 * sizeof(char));
    detected_fields = (char **)malloc(3 * sizeof(char *));
    for (int i = 0; i < 3; i++)
        detected_fields[i] = (char *)malloc(10 * sizeof(char));

    while (getline(&line, &line_size, assembly_file) != -1)
    {
        remove_trailing_nline(line);

        if (line == NULL || strcmp(line, "") == 0) continue;

        token_count = tokenize(line, tokens);
        if (token_count < 0)
            return 1;
        else if (token_count == 0)
            continue;

        // ####TEMP CODE#####
        printf("token_count=%d\n", token_count);
        for (int i = 0; i < token_count; i++)
        {
            printf("tokens[%d]=%s\t", i, tokens[i]);
        }
        printf("\n");
        // ####TEMP CODE#####

        are_tokens_valid = check_valid_tokens(tokens, token_count, detected_instruction, detected_fields);
        if (!are_tokens_valid)
        {
            init_error();
            printf("An error occurred while validating the statement tokens in the following line:\n%s\n", line);
            reset_color();

            return 1;
        }

        if (is_instruction(detected_instruction))
        {
            instruction = form_instruction(detected_instruction, detected_fields, symbol_table, symbol_table_size);

            get_instruction_hex(instruction, instruction_hex);
            int instruction_decimal = hex2int(instruction_hex);
            txt_seg[data_seg_top] = instruction_decimal;
            fprintf(machine_code_file, "%d\n", instruction_decimal);

            data_seg_top++;
        }
        else if (is_directive(detected_instruction))
        {
            int value;
            if (is_numeric(detected_fields[0]))
                value = atoi(detected_fields[0]);
            else if (label_exists(symbol_table, symbol_table_size, detected_fields[0]))
                value = get_label_value(symbol_table, symbol_table_size, detected_fields[0]);
            else
            {
                init_error();
                printf("Label %s was not defined.", detected_fields[0]);
                printf("Error was thrown in line: %s", line);
                reset_color();
                return 1;
            }

            if (strcmp(detected_instruction, ".fill") == 0)
            {
                txt_seg[data_seg_top] = value;
                fprintf(machine_code_file, "%d\n", value);

                data_seg_top++;
            }
            else if (strcmp(detected_instruction, ".space") == 0)
            {
                for (int i = 0; i < value; i++)
                {
                    txt_seg[data_seg_top] = 0;
                    // fprintf(machine_code_file, "%d\n", 0);
                    data_seg_top++;
                }
            }
        }
    }

    return 0;
}

size_t fill_symtab(struct SymbolTable *symbol_table, FILE *inputFile)
{
    char *token;
    size_t line_size = LINE_SIZE;
    char *line = (char *)malloc(line_size * sizeof(char));
    int line_number = 0;
    char delimiter[4] = "\t ";
    size_t symbol_table_size = 0;

    while (getline(&line, &line_size, inputFile) != -1)
    {
        remove_trailing_nline(line);

        if (line == NULL || strcmp(line, "") == 0) continue;

        token = strtok(line, delimiter);

        if (token == NULL) continue;

        if (!is_instruction(token) && !is_directive(token))
        {
            printf("LABEL:%s\n", token);
            (symbol_table + symbol_table_size)->symbol = malloc(strlen(token) * sizeof(char));
            strcpy((symbol_table + symbol_table_size)->symbol, token);
            (symbol_table + symbol_table_size)->value = line_number;
            symbol_table_size++;
        }
        line_number++;
    }
    rewind(inputFile);
    free(line);
    return symbol_table_size;
}

int get_label_value(struct SymbolTable *symbol_table, size_t symbol_table_size, char *label)
{
    for (int i = 0; i, symbol_table_size; i++)
        if (strcmp((symbol_table + i)->symbol, label) == 0) return (symbol_table + i)->value;

    return 0;
}

int label_exists(struct SymbolTable *symbol_table, size_t symbol_table_size, char *label)
{
    for (int i = 0; i, symbol_table_size; i++)
        if (strcmp((symbol_table + i)->symbol, label) == 0) return 1;

    return 0;
}

size_t tokenize(char *line, char **tokens)
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
    char *temp_line = (char *)malloc(line_size * sizeof(char));
    strcpy(temp_line, line);

    curr_token = strtok(temp_line, delimiter);
    while (curr_token != NULL)
    {
        token_count++;
        strcpy(tokens[i++], curr_token);
        printf("%s\n", curr_token);

        if (curr_token[0] == '#') // It's a comment from now on; so we just stop reading tokens here
            return token_count;

        if (token_count == 4)
        {
            // If we read the 4th token and it didn't start with a '#', the line isn't in the correct format
            init_error();
            printf("Error in tokenizing the following line:\n%s\n", line);
            printf("Statement is not in the following format:\n");
            printf("label<white>instruction<white>field0,field1,field2<white>#comments\n");
            reset_color();
            return -1;
        }

        curr_token = strtok(NULL, delimiter);
    }

    return token_count;
}

struct Instruction *form_instruction(char *instruction, char **fields, struct SymbolTable *symbol_table, size_t symbol_table_size)
{
    struct Instruction *compiled_instruction = (struct Instruction *)malloc(sizeof(struct Instruction));

    strcpy(compiled_instruction->inst, instruction);
    compiled_instruction->instType = get_instruction_type(instruction);
    if (compiled_instruction->instType == 0)
    {
        compiled_instruction->rd = atoi(fields[0]);
        compiled_instruction->rs = atoi(fields[1]);
        compiled_instruction->rt = atoi(fields[2]);
    }
    else if (compiled_instruction->instType == 1)
    {
        if (strcpy(instruction, "lui") == 0)
        {
            compiled_instruction->rt = atoi(fields[0]);
            compiled_instruction->rs = 0;
            compiled_instruction->imm = atoi(fields[1]);
        }
        else if (strcpy(instruction, "jalr") == 0)
        {
            compiled_instruction->rt = atoi(fields[0]);
            compiled_instruction->rs = atoi(fields[1]);
            compiled_instruction->imm = 0;
        }
        else if (strcpy(instruction, "lw") == 0 || strcpy(instruction, "sw") == 0 || strcpy(instruction, "beq") == 0)
        {
            if (is_numeric(fields[2]))
                compiled_instruction->imm = atoi(fields[2]);
            else if (label_exists(symbol_table, symbol_table_size, fields[2]))
            {
                compiled_instruction->imm = get_label_value(symbol_table, symbol_table_size, fields[2]);
            }
            else
            {
                init_error();
                printf("Label \"%s\" was not found in the symbol table.\n", fields[2]);
                reset_color();
                return NULL;
            }
        }
        else
        {
            compiled_instruction->rt = atoi(fields[0]);
            compiled_instruction->rs = atoi(fields[1]);
            compiled_instruction->imm = atoi(fields[2]);
        }
    }
    else if (compiled_instruction->instType == 2)
    {
        compiled_instruction->rt = 0;
        if (strcpy(instruction, "halt") == 0)
            compiled_instruction->imm = 0;
        else if (strcpy(instruction, "j") == 0)
        {
            if (label_exists(symbol_table, symbol_table_size, fields[2]))
                compiled_instruction->imm = get_label_value(symbol_table, symbol_table_size, fields[2]);
            else
            {
                init_error();
                printf("Label \"%s\" was not found in the symbol table.\n", fields[2]);
                reset_color();
                return NULL;
            }
        }
    }
    else
    {
        init_error();
        printf("[COMPILER ERROR]: %d is an invalid instruction type number for instruction %s.\n", compiled_instruction->instType, instruction);
        reset_color();
        return NULL;
    }

    return compiled_instruction;
}

void get_instruction_hex(struct Instruction *instruction, char hex[9])
{
    char lower[5];

    hex[8] = '\0';
    hex[0] = '0';
    hex[1] = get_hex_digit(get_instruction_opcode(instruction->inst));

    if (instruction->instType == 0)
    {
        hex[2] = get_hex_digit(instruction->rs);
        hex[3] = get_hex_digit(instruction->rt);
        hex[4] = get_hex_digit(instruction->rd);
        hex[5] = '0';
        hex[6] = '0';
        hex[7] = '0';
    }
    else if (instruction->instType == 1)
    {
        hex[2] = get_hex_digit(instruction->rs);
        hex[3] = get_hex_digit(instruction->rt);
        int2hex16(lower, instruction->imm);
        hex[4] = lower[0];
        hex[5] = lower[1];
        hex[6] = lower[2];
        hex[7] = lower[3];
    }
    else if (instruction->instType == 2)
    {
        hex[2] = '0';
        hex[3] = '0';
        int2hex16(lower, instruction->imm);
        hex[4] = lower[0];
        hex[5] = lower[1];
        hex[6] = lower[2];
        hex[7] = lower[3];
    }
    else
    {
        init_error();
        printf("[COMPILER ERROR]: %d is an invalid instruction type number.\n", instruction->instType);
        reset_color();
    }
}

int check_valid_tokens(char **tokens, size_t token_count, char *instruction, char **fields)
{
    char *fields_token;
    size_t fields_count;
    size_t expected_fields_count;

    // Parses the instruction and fields and checks the format
    switch (token_count)
    {
    case 4:
        // format 1
        strcpy(instruction, tokens[1]);
        fields_token = tokens[2];
        break;
    case 3:
        if (strcmp(tokens[1], "halt") == 0 && tokens[2][0] == '#') // format 4
        {
            strcpy(instruction, "halt");
        }
        else if (tokens[2][0] == '#') // format 3
        {
            strcpy(instruction, tokens[0]);
            fields_token = tokens[1];
        }
        else // format 2
        {
            strcpy(instruction, tokens[1]);
            fields_token = tokens[2];
        }
        break;
    case 2:
        if (strcmp(tokens[1], "halt") == 0) // format 6
        {
            strcpy(instruction, tokens[1]);
        }
        else if (strcmp(tokens[0], "halt") == 0 && tokens[1][0] == '#') // format 7
        {
            strcpy(instruction, tokens[0]);
        }
        else // format 5
        {
            strcpy(instruction, tokens[0]);
            fields_token = tokens[1];
        }
        break;
    case 1:
        strcpy(instruction, tokens[0]);
    default:
        init_error();
        printf("Invalid number of tokens.\n");
        printf("Tokens can only be in a range of 1-4 but received %d\n", token_count);
        reset_color();
        break;
    }

    // #### TEMP CODE ####
    printf("Detected instruction: %s\n", instruction);
    if (fields_token != NULL)
        printf("Fields token: %s\n", fields_token);
    else
        printf("No fields token was detected\n");
    // #### TEMP CODE ####

    if (!is_instruction(instruction) && !is_directive(instruction))
    {
        init_error();
        printf("The token \"%s\", which was detected to be the instruction, is not valid.\n", instruction);
        reset_color();

        return 0;
    }

    fields_count = 0;
    if (fields_token != NULL)
    {
        fields_count = parse_fields_token(fields_token, fields);

        // ####TEMP CODE#####
        printf("fields_count=%d\n", fields_count);
        for (int i = 0; i < fields_count; i++)
        {
            printf("fields[%d]=%s\t", i, fields[i]);
        }
        printf("\n");
        // ####TEMP CODE#####
    }

    expected_fields_count = get_number_of_fields(instruction);

    if (expected_fields_count != fields_count)
    {
        init_error();
        printf("The given fields didn't match the number of expected fields to read.\n");
        printf("Expected: %d, but was given: %d\n", expected_fields_count, fields_count);
        printf("Detected fields: \"%s\"\n", fields_token);
        reset_color();
    }

    return 1;
}

size_t parse_fields_token(char *fields_token, char **parsed)
{
    char *curr_field;
    char *temp_tokens = (char *)malloc(sizeof(fields_token));
    size_t fields_count;
    int i;

    strcpy(temp_tokens, fields_token);

    fields_count = 0;
    i = 0;
    curr_field = strtok(temp_tokens, ",");
    while (curr_field != NULL)
    {
        fields_count++;
        strcpy(parsed[i++], curr_field);

        if (fields_count == 4)
        {
            init_error();
            printf("Too many fields were parsed.");
            reset_color();
            return -1;
        }

        curr_field = strtok(NULL, ",");
    }

    return fields_count;
}

int get_number_of_fields(char *instruction)
{
    if (!is_instruction(instruction) && !is_directive(instruction)) return -1;

    if (strcmp(instruction, "lui") == 0 || strcmp(instruction, "jalr") == 0)
        return 2;
    else if (strcmp(instruction, "j") == 0 || strcmp(instruction, ".fill") == 0 || strcmp(instruction, ".space") == 0)
        return 1;
    else if (strcmp(instruction, "halt") == 0)
        return 0;
    else
        return 3;
}

size_t get_instruction_type(char *instruction)
{
    if (strcmp(instruction, "add") == 0 || strcmp(instruction, "sub") == 0 ||
        strcmp(instruction, "slt") == 0 || strcmp(instruction, "or") == 0 ||
        strcmp(instruction, "nand") == 0)
        return 0;
    else if (strcmp(instruction, "addi") == 0 || strcmp(instruction, "slti") == 0 ||
             strcmp(instruction, "ori") == 0 || strcmp(instruction, "lui") == 0 ||
             strcmp(instruction, "lw") == 0 || strcmp(instruction, "sw") == 0 ||
             strcmp(instruction, "beq") == 0 || strcmp(instruction, "jalr") == 0)
        return 1;
    else if (strcmp(instruction, "j") == 0 || strcmp(instruction, "halt") == 0)
        return 2;
    else
        return 3;
}

int get_instruction_opcode(char *instruction)
{
    for (int i = 0; i < INST_CNT; i++)
        if (strcmp(instructions[i], instruction) == 0) return i;

    return -1;
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

char get_hex_digit(int n)
{
    char hex_table[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
    return hex_table[n % 16];
}

int is_instruction(char *str)
{
    if (str == NULL) return 0;

    for (int i = 0; i < INST_CNT; i++)
        if (strcmp(instructions[i], str) == 0) return 1;

    return 0;
}

int is_directive(char *str)
{
    if (str == NULL) return 0;

    for (int i = 0; i < DRCT_CNT; i++)
        if (strcmp(directives[i], str) == 0) return 1;

    return 0;
}

int is_label_name_valid(char *label)
{
    if (strlen(label) < MAX_LABEL_LEN) return 0;

    if (!isalpha(label[0])) return 0;

    for (int i = 1; i < strlen(label); i++)
        if (!isalpha(label[i]) && !isdigit(label[i])) return 0;

    return 1;
}

int is_numeric(char *str)
{
    size_t size = strlen(str);

    for (int i = 0; i < size; i++)
    {
        if (i == 0 && str[0] == '-') continue;
        if (!isdigit(str[i])) return 0;
    }

    return 1;
}

void remove_trailing_nline(char *str)
{
    if (str == NULL) return;

    size_t size = strlen(str);
    if (str[size - 1] == '\n') str[size - 1] = '\0';
}

void init_error()
{
    printf("\033[0;31m[ERR]: ");
}

void reset_color()
{
    printf("\033[0m");
}