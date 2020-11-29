#ifndef TOKEN_H_DEFINED
#define TOKEN_H_DEFINED

struct token {
    char nome[32];
    char tipo[5];
    int line;
    int column;
};

typedef struct token Token;



#endif