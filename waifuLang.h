#ifndef WAIFULANG_H
#define WAIFULANG_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MEMORY_BLOCK_SIZE 30000

#define MOVE_RIGHT_POINTER_TOKEN "yandere"
#define MOVE_LEFT_POINTER_TOKEN "tsundere"
#define INCREMENT_TOKEN "senpai"
#define DECREMENT_TOKEN "kohai"

#define START_LOOP_TOKEN "hajimeru"
#define END_LOOP_TOKEN "yamete"

#define STDIN_TOKEN "sugoi"
#define STDOUT_TOKEN "ara"

#define JUMP_TOKEN "arigato"
#define ADD_TO_CURRENT_TOKEN "hayai"
#define SUBTRACT_FROM_CURRENT_TOKEN "hikari"

#define IF_TOKEN "saiko"
#define ELSE_IF_TOKEN "saitei"
#define ELSE_TOKEN "shimatta"
#define ENDIF_TOKEN "kudasai"

#define EQUALS_TO_TOKEN "hitoshi"
#define NOT_EQUALS_TO_TOKEN "hitoshikunai"
#define LEQ_TOKEN "ika"
#define GEQ_TOKEN "ijo"
#define GREATER_THAN_TOKEN "oki"
#define LESS_THAN_TOKEN "miman"

typedef enum {
    MOVE_RIGHT_POINTER,
    MOVE_LEFT_POINTER,
    INCREMENT,
    DECREMENT,
    START_LOOP,
    END_LOOP,
    STDIN,
    STDOUT,
    ADD_TO_CURRENT,
    SUBTRACT_FROM_CURRENT,
    JUMP,
    IF,
    ELSE_IF,
    ELSE,
    ENDIF,
    UNKNOWN
} InstructionType;

typedef enum {
    OP_NONE,
    EQUALS_TO,
    NOT_EQUALS_TO,
    LEQ,
    GEQ,
    GREATER_THAN,
    LESS_THAN,
} OperatorType;

typedef enum {
    NONE,
    INTEGER,
    MEMORY_OFFSET
} OperandType;

typedef struct {
    OperandType type;
    int int_value;
    int memory_offset;
} Operand;

typedef struct {
    OperatorType type;
    Operand operand;
} Operator;

typedef struct {
    InstructionType type;
    char* raw;
    Operator operator;
} Instruction;

typedef struct {
    char memory[MEMORY_BLOCK_SIZE];
    char* pointer;
    int pointer_index;
    int condition_stack[MEMORY_BLOCK_SIZE];
    int condition_stack_top;
} ProgramState;

Instruction* tokenize_source(const char* source, size_t* instruction_count);
void parse_operator(const char** end, Instruction* instr);
void parse_operand(const char** end, Instruction* instr);
void validate_tokens(Instruction* instructions, size_t instruction_count);
void execute_program(Instruction* instructions, size_t instruction_count, ProgramState* state);
void handle_error(const char* message);
int evaluate_condition(Operator opr, Operand op, ProgramState* state);
void skip_to_else_or_endif(size_t* pc, Instruction* instructions, size_t instruction_count);
void skip_to_endif(size_t* pc, Instruction* instructions, size_t instruction_count);
void skip_to_matching_end_loop(size_t* pc, Instruction* instructions, size_t instruction_count);

Instruction* tokenize_source(const char* source, size_t* instruction_count) {
    size_t capacity = 10, count = 0;
    Instruction* instructions = (Instruction*)malloc(capacity * sizeof(Instruction));

    if (!instructions) {
        perror("Memory allocation failed");
        exit(EXIT_FAILURE);
    }

    const char* start = source;
    const char* end = source;

    while (*end != '\0') {
        // Move the `end` pointer until a space or end of the line is found
        while (*end != '\0' && *end != ' ' && *end != '\t' && *end != '\n') {
            end++;
        }

        size_t token_length = end - start;
        if (token_length > 0) {
            // Extract the token
            char* token = (char*)malloc(token_length + 1);
            strncpy(token, start, token_length);
            token[token_length] = '\0';

            // Determine instruction type
            Instruction instr;
            instr.raw = token;

            if (strcmp(token, MOVE_RIGHT_POINTER_TOKEN) == 0) {
                instr.type = MOVE_RIGHT_POINTER;
            } else if (strcmp(token, MOVE_LEFT_POINTER_TOKEN) == 0) {
                instr.type = MOVE_LEFT_POINTER;
            } else if (strcmp(token, INCREMENT_TOKEN) == 0) {
                instr.type = INCREMENT;
            } else if (strcmp(token, DECREMENT_TOKEN) == 0) {
                instr.type = DECREMENT;
            } else if (strcmp(token, START_LOOP_TOKEN) == 0) {
                instr.type = START_LOOP;
            } else if (strcmp(token, END_LOOP_TOKEN) == 0) {
                instr.type = END_LOOP;
            } else if (strcmp(token, STDIN_TOKEN) == 0) {
                instr.type = STDIN;
            } else if (strcmp(token, STDOUT_TOKEN) == 0) {
                instr.type = STDOUT;
            } else if (strcmp(token, JUMP_TOKEN) == 0) {
                instr.type = JUMP;
                parse_operand(&end, &instr);
            } else if (strcmp(token, ADD_TO_CURRENT_TOKEN) == 0) {
                instr.type = ADD_TO_CURRENT;
            } else if (strcmp(token, SUBTRACT_FROM_CURRENT_TOKEN) == 0) {
                instr.type = SUBTRACT_FROM_CURRENT;
            } else if (strcmp(token, IF_TOKEN) == 0) {
                instr.type = IF;
                parse_operator(&end, &instr);
                parse_operand(&end, &instr);
            } else if (strcmp(token, ELSE_IF_TOKEN) == 0) {
                instr.type = ELSE_IF;
                parse_operator(&end, &instr);
                parse_operand(&end, &instr);
            } else if (strcmp(token, ELSE_TOKEN) == 0) {
                instr.type = ELSE;
            } else if (strcmp(token, ENDIF_TOKEN) == 0) {
                instr.type = ENDIF;
            } else {
                instr.type = UNKNOWN;
            }
            // Add to instructions
            if (count >= capacity) {
                capacity *= 2;
                instructions = (Instruction*)realloc(instructions, capacity * sizeof(Instruction));
                if (!instructions) {
                    perror("Memory allocation failed");
                    exit(EXIT_FAILURE);
                }
            }
            instructions[count++] = instr;
        }

        // Skip spaces/tabs/newlines
        while (*end == ' ' || *end == '\t' || *end == '\n') {
            end++;
        }
        start = end;
    }

    *instruction_count = count;
    return instructions;
}

void parse_operator(const char** end, Instruction* instr) {
    // Read the operator
    while (**end == ' ' || **end == '\t' || **end == '\n') {
        (*end)++;
    }
    const char* start = *end;
    const char* instruction = instr->raw;

    while (**end != '\0' && **end != ' ' && **end != '\t' && **end != '\n') {
        (*end)++;
    }

    size_t operator_length = *end - start;
    if (operator_length > 0) {
        char* operator_token = (char*)malloc(operator_length + 1);
        strncpy(operator_token, start, operator_length);
        operator_token[operator_length] = '\0';

        if (strcmp(operator_token, EQUALS_TO_TOKEN) == 0) {
            instr->operator.type = EQUALS_TO;
        } else if (strcmp(operator_token, NOT_EQUALS_TO_TOKEN) == 0) {
            instr->operator.type = NOT_EQUALS_TO;
        } else if (strcmp(operator_token, LEQ_TOKEN) == 0) {
            instr->operator.type = LEQ;
        } else if (strcmp(operator_token, GEQ_TOKEN) == 0) {
            instr->operator.type = GEQ;
        } else if (strcmp(operator_token, GREATER_THAN_TOKEN) == 0) {
            instr->operator.type = GREATER_THAN;
        } else if (strcmp(operator_token, LESS_THAN_TOKEN) == 0) {
            instr->operator.type = LESS_THAN;
        } else {
            char* err_msg = (char*)malloc(strlen(instruction) + strlen("Invalid operator for : ") + strlen(operator_token) + 1);
            sprintf(err_msg, "Invalid operator for %s: %s\n", instruction, operator_token);
            handle_error(err_msg);
        }
        free(operator_token);
    } else {
        char* err_msg = (char*)malloc(strlen(instruction) + strlen("Missing operator for ") + 1);
        sprintf(err_msg, "Missing operator for %s!", instruction);
        handle_error(err_msg);
    }
}

void parse_operand(const char** end, Instruction* instr) {
    // Read the operand
    while (**end == ' ' || **end == '\t' || **end == '\n') {
        (*end)++;
    }
    const char* start = *end;
    const char* instruction = instr->raw;

    while (**end != '\0' && **end != ' ' && **end != '\t' && **end != '\n') {
        (*end)++;
    }

    size_t operand_length = *end - start;
    if (operand_length > 0) {
        char* operand_token = (char*)malloc(operand_length + 1);
        strncpy(operand_token, start, operand_length);
        operand_token[operand_length] = '\0';

        if (isdigit(*operand_token)) {
            int value = atoi(operand_token);
            if (value < 0 || value >= MEMORY_BLOCK_SIZE) {
                handle_error("Memory offset out of bound!");
            }
            instr->operator.operand.type = INTEGER;
            instr->operator.operand.int_value = value;
        } else {
            char* err_msg = (char*)malloc(strlen(instruction) + strlen("Invalid operand for : ") + strlen(operand_token) + 1);
            sprintf(err_msg, "Invalid operand for %s: %s\n", instruction, operand_token);
            handle_error(err_msg);
        }
        free(operand_token);
    } else {
        char* err_msg = (char*)malloc(strlen(instruction) + strlen("Missing operand for ") + 1);
        sprintf(err_msg, "Missing operand for %s!", instruction);
        handle_error(err_msg);
    }
}


void validate_tokens(Instruction* instructions, size_t instruction_count) {
    int loop_depth = 0;
    int if_depth = 0;
    for (size_t i = 0; i < instruction_count; i++) {
        if (instructions[i].type == UNKNOWN) {
            const char* err_token = instructions[i].raw;
            char* err_msg = (char*)malloc(strlen(err_token) + strlen("baka: Unknown token ") + 2 + 1); // 2 for quotes and 1 for null terminator
            sprintf(err_msg, "baka: Unknown token '%s'", err_token);
            handle_error(err_msg); // free-ing err_msg is not necessary since the program will exit
        } else if (instructions[i].type == START_LOOP) {
            loop_depth++;
        } else if (instructions[i].type == END_LOOP) {
            loop_depth--;
            if (loop_depth < 0) {
                handle_error("baka: Too much yamete statements!");
            }
        } else if (instructions[i].type == IF) {
            if_depth++;
        } else if (instructions[i].type == ENDIF) {
            if_depth--;
            if (if_depth < 0) {
                handle_error("baka: Too much kudasai statements!");
            }
        }
    }

    if (loop_depth != 0) {
        handle_error("baka: Too much hajimeru statements!");
    }
    if (if_depth != 0) {
        handle_error("baka: Too much saiko statements!");
    }
}

void execute_program(Instruction* instructions, size_t instruction_count, ProgramState* state) {
    size_t pc = 0;
    size_t loop_stack[MEMORY_BLOCK_SIZE];
    int loop_index = -1;

    while (pc < instruction_count) {
        Instruction instr = instructions[pc];
        Operator operator = instr.operator;
        Operand operand = instr.operator.operand;

        switch (instr.type) {
            case MOVE_RIGHT_POINTER:
                if (state->pointer_index >= MEMORY_BLOCK_SIZE - 1) handle_error("baka: Reached end of memory block!");
                state->pointer++;
                state->pointer_index++;
                break;
            case MOVE_LEFT_POINTER:
                if (state->pointer_index <= 0) handle_error("baka: Reached beginning of memory block!");
                state->pointer--;
                state->pointer_index--;
                break;
            case INCREMENT:
                (*state->pointer)++;
                break;
            case DECREMENT:
                (*state->pointer)--;
                break;
            case JUMP:
                state->pointer = &state->memory[operand.int_value];
                state->pointer_index = operand.int_value;
                break;
            case ADD_TO_CURRENT:
                if (state->pointer_index >= MEMORY_BLOCK_SIZE - 1) handle_error("baka: Reached end of memory block!");
                (*state->pointer) += *(state->pointer + 1);
                break;
            case SUBTRACT_FROM_CURRENT:
                if (state->pointer_index >= MEMORY_BLOCK_SIZE - 1) handle_error("baka: Reached end of memory block!");
                (*state->pointer) -= *(state->pointer + 1);
                break;
            case IF:
                state->condition_stack[++state->condition_stack_top] = 0; 
                if (!evaluate_condition(operator, operand, state)) {
                    skip_to_else_or_endif(&pc, instructions, instruction_count);
                } else {
                    state->condition_stack[state->condition_stack_top] = 1;  // Mark as executed
                }
                break;
            case ELSE_IF:
                if (state->condition_stack[state->condition_stack_top] == 1) {
                    skip_to_endif(&pc, instructions, instruction_count);
                } else if (!evaluate_condition(operator, operand, state)) {
                    skip_to_else_or_endif(&pc, instructions, instruction_count);
                } else {
                    state->condition_stack[state->condition_stack_top] = 1;
                }
                break;
            case ELSE:
                if (state->condition_stack[state->condition_stack_top] == 1) {
                    skip_to_endif(&pc, instructions, instruction_count);
                }
                break;
            case ENDIF:
                if (state->condition_stack_top >= 0) {
                    state->condition_stack[state->condition_stack_top--] = 0;
                }
                break;
            case START_LOOP:
                if (*state->pointer == 0) {
                    skip_to_matching_end_loop(&pc, instructions, instruction_count);
                } else {
                    loop_stack[++loop_index] = pc;
                }
                break;
            case END_LOOP:
                if (*state->pointer != 0) {
                    pc = loop_stack[loop_index];
                } else {
                    loop_index--;
                }
                break;
            case STDIN:
                // Due to getchar(), only a single character can be read at a time.
                // TODO: Implement a better way to read input
                *state->pointer = getchar();
                break;
            case STDOUT:
                putchar(*state->pointer);
                break;
            default:
                handle_error("baka: Unknown error occured!");
        }
        pc++;
    }
}

int evaluate_condition(Operator opr, Operand op, ProgramState* state) {
    char current = *state->pointer;
    int target_value = state->memory[op.int_value];

    switch (opr.type) {
        case EQUALS_TO:
            return current == target_value;
        case NOT_EQUALS_TO:
            return current != target_value;
        case LEQ:
            return current <= target_value;
        case GEQ:
            return current >= target_value;
        case GREATER_THAN:
            return current > target_value;
        case LESS_THAN:
            return current < target_value;
        default:
            return current < target_value; // doesnt affect the program tho
    }
}

void skip_to_else_or_endif(size_t* pc, Instruction* instructions, size_t instruction_count) {
    while (*pc < instruction_count) {
        (*pc)++;
        if (instructions[*pc].type == ELSE || instructions[*pc].type == ELSE_IF || instructions[*pc].type == ENDIF) {
            (*pc)--;  // Go back one to point at the ELSE/ELSE_IF/ENDIF
            return;
        }
    }
}

void skip_to_endif(size_t* pc, Instruction* instructions, size_t instruction_count) {
    while (*pc < instruction_count) {
        (*pc)++;
        if (instructions[*pc].type == ENDIF) {
            (*pc)--;  // Go back one to point at the ENDIF
            return;
        }
    }
}

void skip_to_matching_end_loop(size_t* pc, Instruction* instructions, size_t instruction_count) {
    int loop_depth = 1;
    while (loop_depth > 0 && ++(*pc) < instruction_count) {
        if (instructions[*pc].type == START_LOOP) {
            loop_depth++;
        } else if (instructions[*pc].type == END_LOOP) {
            loop_depth--;
        }
    }
}

void handle_error(const char* message) {
    fprintf(stderr, "%s\n", message);
    exit(1);
}

#endif
