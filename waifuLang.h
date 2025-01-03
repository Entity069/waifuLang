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
#define ENDIF_TOKEN "yamete"

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
    JUMP,
    ADD_TO_CURRENT,
    SUBTRACT_FROM_CURRENT,
    IF,
    ELSE_IF,
    ELSE,
    ENDIF,
    EQUALS_TO,
    NOT_EQUALS_TO,
    LEQ,
    GEQ,
    GREATER_THAN,
    LESS_THAN,
    UNKNOWN
} InstructionType;

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
    InstructionType type;
    char* raw;
    Operand operand;
} Instruction;

typedef struct {
    char memory[MEMORY_BLOCK_SIZE];
    char* pointer;
    int pointer_index;
    int condition_stack[MEMORY_BLOCK_SIZE];
    int condition_stack_top;
} ProgramState;

Instruction* tokenize_source(const char* source, size_t* instruction_count);
void validate_tokens(Instruction* instructions, size_t instruction_count);
void execute_program(Instruction* instructions, size_t instruction_count, ProgramState* state);
void handle_error(const char* message);
int evaluate_condition(Operand op, ProgramState* state);
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
            instr.operand.type = NONE; // Default operand type

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

                // Read the operand
                while (*end == ' ' || *end == '\t' || *end == '\n') {
                    end++;
                }
                start = end;

                while (*end != '\0' && *end != ' ' && *end != '\t' && *end != '\n') {
                    end++;
                }

                size_t operand_length = end - start;
                if (operand_length > 0) {
                    char* operand_token = (char*)malloc(operand_length + 1);
                    strncpy(operand_token, start, operand_length);
                    operand_token[operand_length] = '\0';

                    if (isdigit(*operand_token)) {
                        int value = atoi(operand_token);
                        if (value < 0 || value >= MEMORY_BLOCK_SIZE) {
                            handle_error(("Invalid operand for arigato: %s\n", operand_token));
                        }
                        instr.operand.type = INTEGER;
                        instr.operand.int_value = value;
                    } else {
                        handle_error(("Invalid operand for arigato: %s\n", operand_token));
                    }
                    free(operand_token);
                } else {
                    handle_error("Missing operand for arigato");
                }
            } else if (strcmp(token, ADD_TO_CURRENT_TOKEN) == 0) {
                instr.type = ADD_TO_CURRENT;
            } else if (strcmp(token, SUBTRACT_FROM_CURRENT_TOKEN) == 0) {
                instr.type = SUBTRACT_FROM_CURRENT;
            } else if (strcmp(token, IF_TOKEN) == 0) {
                instr.type = IF;
            } else if (strcmp(token, ELSE_IF_TOKEN) == 0) {
                instr.type = ELSE_IF;
            } else if (strcmp(token, ELSE_TOKEN) == 0) {
                instr.type = ELSE;
            } else if (strcmp(token, ENDIF_TOKEN) == 0) {
                instr.type = ENDIF;
            } else if (strcmp(token, EQUALS_TO_TOKEN) == 0) {
                instr.type = EQUALS_TO;
            } else if (strcmp(token, NOT_EQUALS_TO_TOKEN) == 0) {
                instr.type = NOT_EQUALS_TO;
            } else if (strcmp(token, LEQ_TOKEN) == 0) {
                instr.type = LEQ;
            } else if (strcmp(token, GEQ_TOKEN) == 0) {
                instr.type = GEQ;
            } else if (strcmp(token, GREATER_THAN_TOKEN) == 0) {
                instr.type = GREATER_THAN;
            } else if (strcmp(token, LESS_THAN_TOKEN) == 0) {
                instr.type = LESS_THAN;
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


void validate_tokens(Instruction* instructions, size_t instruction_count) {
    int loop_depth = 0;

    for (size_t i = 0; i < instruction_count; i++) {
        if (instructions[i].type == UNKNOWN) {
            handle_error(("Unknown token %s", instructions[i].raw));
        } else if (instructions[i].type == START_LOOP) {
            loop_depth++;
        } else if (instructions[i].type == END_LOOP) {
            loop_depth--;
            if (loop_depth < 0) {
                handle_error("Unknown error occured!");
            }
        }
    }

    if (loop_depth != 0) {
        handle_error("Unknown error occured!");
    }
}

void execute_program(Instruction* instructions, size_t instruction_count, ProgramState* state) {
    size_t pc = 0;
    size_t loop_stack[MEMORY_BLOCK_SIZE];
    int loop_index = -1;

    while (pc < instruction_count) {
        Instruction instr = instructions[pc];
        Operand op = instr.operand;

        switch (instr.type) {
            case MOVE_RIGHT_POINTER:
                if (state->pointer_index >= MEMORY_BLOCK_SIZE - 1) handle_error("baka");
                state->pointer++;
                state->pointer_index++;
                break;
            case MOVE_LEFT_POINTER:
                if (state->pointer_index <= 0) handle_error("baka");
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
                if (op.type != INTEGER || op.int_value < 0 || op.int_value >= MEMORY_BLOCK_SIZE) {
                    handle_error("baka");
                }
                state->pointer = &state->memory[op.int_value];
                state->pointer_index = op.int_value;
                break;
            case ADD_TO_CURRENT:
                if (state->pointer_index >= MEMORY_BLOCK_SIZE - 1) handle_error("baka");
                (*state->pointer) += *(state->pointer + 1);
                break;
            case SUBTRACT_FROM_CURRENT:
                if (state->pointer_index >= MEMORY_BLOCK_SIZE - 1) handle_error("baka");
                (*state->pointer) -= *(state->pointer + 1);
                break;
            case IF:
                if (!evaluate_condition(op, state)) {
                    skip_to_else_or_endif(&pc, instructions, instruction_count);
                }
                break;
            case ELSE_IF:
                if (state->condition_stack[state->condition_stack_top] == 1 || !evaluate_condition(op, state)) {
                    skip_to_else_or_endif(&pc, instructions, instruction_count);
                } else {
                    state->condition_stack[state->condition_stack_top] = 1;
                }
                break;
            case ELSE:
                if (state->condition_stack[state->condition_stack_top] == 1) {
                    skip_to_endif(&pc, instructions, instruction_count);
                } else {
                    state->condition_stack[state->condition_stack_top] = 1;
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
                *state->pointer = getchar();
                break;
            case STDOUT:
                putchar(*state->pointer);
                break;
            default:
                handle_error("baka");
        }

        pc++;
    }
}

int evaluate_condition(Operand op, ProgramState* state) {
    char current = *state->pointer;
    char target = *(state->pointer + op.memory_offset);

    switch (op.type) {
        case EQUALS_TO:
            return current == target;
        case NOT_EQUALS_TO:
            return current != target;
        case LEQ:
            return current <= target;
        case GEQ:
            return current >= target;
        case GREATER_THAN:
            return current > target;
        // case LESS_THAN:
        //     return current < target;
        default:
            return current < target;
    }
}

void skip_to_else_or_endif(size_t* pc, Instruction* instructions, size_t instruction_count) {
    int depth = 1;
    while (depth > 0 && ++(*pc) < instruction_count) {
        if (instructions[*pc].type == IF || instructions[*pc].type == ELSE_IF) {
            depth++;
        } else if (instructions[*pc].type == ELSE || instructions[*pc].type == ENDIF) {
            depth--;
        }
    }
}

void skip_to_endif(size_t* pc, Instruction* instructions, size_t instruction_count) {
    int depth = 1;
    while (depth > 0 && ++(*pc) < instruction_count) {
        if (instructions[*pc].type == IF) {
            depth++;
        } else if (instructions[*pc].type == ENDIF) {
            depth--;
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
