#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "automaton.h"
#include "token.h"


char *getTokenType(ReturnState state);
Token *getTokens(FILE *source_code, char *automaton_path, int *n_tokens);

int main(int argc, char* argv[]) {
    char source_code_path[128];
    char automaton_path[128];
    FILE *source_code;
    Token *tokens;
    int n_tokens;

    if (argc != 3) {
        printf("Usage: <application name> <source code path> <automaton path>\n");
        exit(1);
    }
    strncpy(source_code_path, argv[1], 128);
    strncpy(automaton_path, argv[2], 128);
    printf("Parsing source code '%s' using automaton '%s'\n", source_code_path, automaton_path);

    source_code = fopen(source_code_path, "r");

    tokens = getTokens(source_code, automaton_path, &n_tokens);

    for(int i = 0; i < n_tokens; i++) {
        free(tokens[i]);
    }
    free(tokens);
    fclose(source_code);
    return EXIT_SUCCESS;
}

Token *getTokens(FILE *source_code, char *automaton_path, int *n_tokens) {
    char car;
    int current_line = 0;
    int current_column = 0;
    int current_token = 0;

    Automaton automaton = create_automaton(automaton_path);
    Token *tokens = (Token*) malloc(1000 * sizeof(Token));
    tokens[current_token] = create_token(current_line, current_column);
    ReturnState state;
    while((car = fgetc(source_code)) != EOF) {
        state = consume(automaton, car);
        if (add_char(tokens[current_token], car) < 0) {
            printf("Max token name limit reached for '%s'\n", get_name(tokens[current_token]));
            printf("Line: %d, Column: %d\n", current_line, current_column);
        }

        if (state->tokenID < 0) {
            printf("Invalid token: '%s'\n", get_name(tokens[current_token]));
            printf("Line: %d, Column: %d\n", get_line(tokens[current_token]), get_column(tokens[current_token]));
            exit(1);
        } else if (state->tokenID == 0) {
            current_column++;
        } else {
            if(state->returnCar) {
                fseek(source_code, -1, SEEK_CUR);
            } else {
                remove_char(tokens[current_token]);
            }
            strncpy(get_type(tokens[current_token]), getTokenType(state), sizeof(get_type(tokens[current_token])));
            tokens[++current_token] = create_token(current_line, current_column + 1);
        }
    }

    destroy_automaton(automaton);
    *n_tokens = current_token - 1;
    return tokens;
}

char *getTokenType(ReturnState state) {
    switch (state->tokenID)
    {
    case PROG:
        return "PROG";
    case ID:
        return "ID";
    case OCURL:
        return "OCURL";
    case CCURL:
        return "CCURL";
    case IF:
        return "IF";
    case THEN:
        return "THEN";
    case ELSE:
        return "ELSE";
    case RELOP:
        return "RELOP";
    case TYPE:
        return "TYPE";
    case OP:
        return "OP";
    case OPAR:
        return "OPAR";
    case CPAR:
        return "CPAR";
    case COLON:
        return "COLON";
    case WHILE:
        return "WHILE";
    case DO:
        return "DO";
    case CONST:
        return "CONST";
    case COMMT:
        return "COMMT";
    default:
        return NULL;
    }
}
