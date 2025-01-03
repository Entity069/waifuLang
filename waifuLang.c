#include "waifuLang.h"

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Usage: %s <source_file>\n", argv[0]);
        return 1;
    }

    FILE* file = fopen(argv[1], "r");
    if (!file) {
        fprintf(stderr, "Error: Could not open file %s\n", argv[1]);
        return 1;
    }

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    rewind(file);

    char* source = (char*)malloc(file_size + 1);
    fread(source, 1, file_size, file);
    source[file_size] = '\0';
    fclose(file);

    size_t instruction_count = 0;
    Instruction* instructions = tokenize_source(source, &instruction_count);
    free(source);

    validate_tokens(instructions, instruction_count);

    ProgramState state = {.memory = {0}, .pointer = state.memory};
    execute_program(instructions, instruction_count, &state);

    for (size_t i = 0; i < instruction_count; i++) {
        free(instructions[i].raw);
    }
    free(instructions);

    return 0;
}
