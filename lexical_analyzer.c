#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "automaton.h"
#include "token.h"


// Token *analyze();

int main(int argc, char* argv[]) {
    char source_code_path[128];
    char automaton_path[128];
    FILE *source_code;
    Automaton automaton;
    Token tokens[1000];

    if (argc != 3) {
        printf("Usage: <application name> <source code path> <automaton path>\n");
        exit(1);
    }
    strncpy(source_code_path, argv[1], 128);
    strncpy(automaton_path, argv[2], 128);
    printf("Parsing source code '%s' using automaton '%s'\n", source_code_path, automaton_path);

    source_code = fopen(source_code_path, "r");

    char car;
    char token[32];
    char tokenSize = 0;
    int current_token = 0;
    int current_line = 0;
    int current_column = 0;
    automaton = create_automaton(automaton_path);
    while((car = fgetc(source_code)) != EOF) {
        ReturnState state;
        if (car == "\n") {
            current_line++;
            continue;
        } else {
            current_column++;
        }
        if (car == ' ') {
            
            continue;
        }

        state = consume(automaton, car);
        token[tokenSize++] = car;
        if(state->returnCar) {
            fseek(source_code, -1, SEEK_CUR);
        }

        // if 
    }

    destroy_automaton(automaton);
    fclose(source_code);
    return EXIT_SUCCESS;
}

