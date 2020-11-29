#ifndef TOKEN_H_DEFINED
#define TOKEN_H_DEFINED

typedef struct token* Token;

Token *create_token(int line, int column);
int add_char(Token token, char ch); // Add a char to token name
int remove_char(Token token); // Remove the last char from token name

char *get_name(Token token);
int get_size(Token token);
int get_type(Token token);
int get_line(Token token);
int get_column(Token token);

#endif