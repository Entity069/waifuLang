#ifndef WAIFULANG_H
#define WAIFULANG_H

#include <stddef.h>

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

#endif
