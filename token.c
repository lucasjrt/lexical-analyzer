#include <string.h>
#include "token.h"

struct token {
    char name[32];
    char size;
    char type[5];
    int line;
    int column;
};

Token create_token(int line, int column) {
    Token token = (Token) malloc(sizeof(struct token));
    token->name[0] = '\0';
    token->type[0] = '\0';
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
