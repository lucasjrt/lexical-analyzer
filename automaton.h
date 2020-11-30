#ifndef AUTOMATON_H_DEFINED
#define AUTOMATON_H_DEFINED

#define PROG  1
#define ID    2
#define OCURL 3
#define CCURL 4
#define IF    5
#define THEN  6
#define ELSE  7
#define RELOP 8
#define TYPE  9
#define OP    10
#define OPAR  11
#define CPAR  12
#define COLON 13
#define WHILE 14
#define DO    15
#define CONST 16
#define COMMT 17

struct returnState {
    char token_name[32];
    int tokenType;
    int returnCar;
};

typedef struct automaton* Automaton;
typedef struct returnState* ReturnState;
 
Automaton create_automaton(char* caminho);
int reset_automaton(Automaton *a);
/*
 * Returns a struct containing a value indicating if the car need to be returned
 * if returnCar is 0, then the car is not to be returned, else it must return.
 * The other attribute of the struct is tokenType, it indicates the ID of the
 * token, or if the current state is not a final state, it returns 0, or else if
 * the token is not valid, it returns a value < 0.
 */
ReturnState consume(Automaton automaton, char car);

#endif

































/*


// Get an automaton and a char, and if the character is accepted, returns 0, else return -1 *
ReturnState consume(Automaton a, char c); // <-----

int readStates();
int readAlphabet();
int readTransitions();
int readBeginState();
int readFinalState();
int final_state(state);

int parseDelta() {
    read("(");
    delta->state0 = readState();
    read(",'");
    regex = readRegex();
    read("',");
    delta->state1 = readState();
    read(")");
}

Estrutura de um estado:
estado_nao_final: string
estado_final: (string,string,boolean)
estado_final: (id,LE,retorna_carro)

////// dat /////
estados
1,2,3,4,5,(6,LT,t),(7,NE,f)
delta
(1,"[a-z]",2)
string,string,string
///////////////

///// funcoes que precisam //////


/////////////////////////////////

char[] token;
char[][] tokens;

(1,'[a-z]',2), (2,'[^/]',3)
match('a', "[a-z]") => return 0; // 0 válido, < 0 inválido.
match('1', "[a-z]") => return -1;
token[i] = read();

// token: a
tokens[i] = token;

Automaton create_automaton();

// .c

Automaton a = create_automaton();

while...
consume(a, character)


*/

