#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "token.h"
#include "automaton.h"

struct token {
    char name[32];
    int size;
    int type;
    int line;
    int column;
};

Token create_token(int line, int column) {
    Token token = (Token) malloc(sizeof(struct token));
    token->name[0] = '\0';
    token->type = -1;
    token->size = 0;
    token->line = line;
    token->column = column;
    return token;
}

int add_char(Token token, char ch) {
    if(token->size >= 31) {
        return -1;
    }

    token->name[token->size] = ch;
    token->name[token->size + 1] = '\0';
    token->size++;
    return 0;
}

int remove_char(Token token) {
    if (token->size == 0) {
        return -1;
    }

    token->name[token->size] = '\0';
    token->size--;
    return 1;
}

char *get_name(Token token) {
    return token->name;
}

int get_size(Token token) {
    return token->size;
}

int get_type(Token token) {
    return token->type;
}

int get_line(Token token) {
    return token->line;
}

int get_column(Token token) {
    return token->column;
}

int set_name(Token token, char *name){
    if(name == NULL){
        return -1;
    }
    strncpy(token->name, name, 32);
    return 0;
}

int set_type(Token token, int type){
    token->type = type;
    return 0;
}

void print_token(Token token){
    printf("Name: %s\n", token->name);
    printf("Type: %s\n", getTokenType(token));
    printf("Token size: %d\n", token->size);
    printf("Line: %d\n", token->line);
    printf("Column: %d\n", token->column);
}

char *getTokenType(Token token) {
    switch (token->type)
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
