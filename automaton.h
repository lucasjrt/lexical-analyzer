#ifndef AUTOMATON_H_DEFINED
#define AUTOMATON_H_DEFINED

struct returnState {
     int tokenID;
     int returnCar;
 };

 typedef struct automaton* Automaton;
 typedef struct returnState ReturnState;
 Automaton carrega_automato(char* caminho);

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

