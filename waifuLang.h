#ifndef WAIFULANG_H
#define WAIFULANG_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MEMORY_BLOCK_SIZE 30000

#define MOVE_RIGHT_POINTER_TOKEN "yandere"
#define MOVE_LEFT_POINTER_TOKEN "tsundere"
#define INCREMENT_TOKEN "senpai"
#define DECREMENT_TOKEN "kohai"
#define START_LOOP_TOKEN "hajimeru"
#define END_LOOP_TOKEN "yamete"
#define STDIN_TOKEN "sugoi"
#define STDOUT_TOKEN "ara"

typedef enum {
    MOVE_RIGHT_POINTER,
    MOVE_LEFT_POINTER,
    INCREMENT,
    DECREMENT,
    START_LOOP,
    END_LOOP,
    STDIN,
    STDOUT,
    UNKNOWN
} InstructionType;

typedef struct {
    InstructionType type;
    char* raw;
} Instruction;

typedef struct {
    char memory[MEMORY_BLOCK_SIZE];
    char* pointer;
} ProgramState;

Instruction* tokenize_source(const char* source, size_t* instruction_count);
void validate_tokens(Instruction* instructions, size_t instruction_count);
void execute_program(Instruction* instructions, size_t instruction_count, ProgramState* state);
void handle_error(const char* message);

Instruction* tokenize_source(const char* source, size_t* instruction_count) {
    size_t capacity = 10, count = 0;
    Instruction* instructions = (Instruction*)malloc(capacity * sizeof(Instruction));

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
            InstructionType type;
            if (strcmp(token, MOVE_RIGHT_POINTER_TOKEN) == 0) type = MOVE_RIGHT_POINTER;
            else if (strcmp(token, MOVE_LEFT_POINTER_TOKEN) == 0) type = MOVE_LEFT_POINTER;
            else if (strcmp(token, INCREMENT_TOKEN) == 0) type = INCREMENT;
            else if (strcmp(token, DECREMENT_TOKEN) == 0) type = DECREMENT;
            else if (strcmp(token, START_LOOP_TOKEN) == 0) type = START_LOOP;
            else if (strcmp(token, END_LOOP_TOKEN) == 0) type = END_LOOP;
            else if (strcmp(token, STDIN_TOKEN) == 0) type = STDIN;
            else if (strcmp(token, STDOUT_TOKEN) == 0) type = STDOUT;
            else type = UNKNOWN;

            // Add to instructions
            if (count >= capacity) {
                capacity *= 2;
                instructions = (Instruction*)realloc(instructions, capacity * sizeof(Instruction));
            }
            instructions[count++] = (Instruction){.type = type, .raw = token};
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

        switch (instr.type) {
            case MOVE_RIGHT_POINTER:
                state->pointer++;
                break;
            case MOVE_LEFT_POINTER:
                state->pointer--;
                break;
            case INCREMENT:
                (*state->pointer)++;
                break;
            case DECREMENT:
                (*state->pointer)--;
                break;
            case START_LOOP:
                if (*state->pointer == 0) {
                    int loop_depth = 1;
                    while (loop_depth > 0) {
                        pc++;
                        if (pc >= instruction_count) handle_error("baka");
                        if (instructions[pc].type == START_LOOP) loop_depth++;
                        else if (instructions[pc].type == END_LOOP) loop_depth--;
                    }
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
                handle_error(instructions[pc].raw);
        }

        pc++;
    }
}

void handle_error(const char* message) {
    fprintf(stderr, "baka : %s\n", message);
    exit(1);
}

#endif
