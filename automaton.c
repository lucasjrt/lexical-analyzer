#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "automaton.h"

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

struct state{
    char state_name[15];
    int token_type;
    char token_name[15];
    int return_car;
};
typedef struct state State;

struct delta {
    State src;
    State dest;
    char transition[15];
};
typedef struct delta Delta;

int main(){
    Automaton a;
    a = carrega_automato("Automaton_new_model.dat");
    //mostrarAutomato(a);
}

struct automaton {
    State states[300];
    int n_states;
    Delta transition_functions[300];
    int n_functions;
    State begin_state;
    State final_states[300];
    int n_final_states;
    int current_state;
};

int carrega_final(Automaton *a, char *final);
int carrega_inicial(Automaton *a, char *inicial);
void carrega_estados(Automaton *a, char *estados);
int carrega_delta(Automaton *a, char *delta);
int pertence_estado(Automaton a, char *estado);
void atribui(char *a, char *b);
void mostrarAutomato(Automaton a);
State *inserirEstado(Automaton *a, char *state_name, char *token_type, char *token_name, int return_car);
void mostrarEstados(Automaton a);
int getIntTokenType(char *tokenType);


Automaton carrega_automato(char* caminho) {
    FILE *f = fopen(caminho, "r");
    if(f == NULL) {
      printf("File not found\n");
      return NULL;
    }
    Automaton a = (Automaton) malloc(sizeof(struct automaton));
    char temp[300];
    //Carrega os estados do autômato
    fscanf(f, "%s", temp); //Lê "estados"
    if(!strcmp(temp, "estados")) {
        fscanf(f, "%s", temp); //Lê os estados
        carrega_estados(&a, temp);
        //mostrarEstados(a);
        //return 0;
        printf("%d states loaded: \n", a->n_states);
        int i;
        for(i = 0; i < a->n_states; i++) {
            printf("%s%s", a->states[i].state_name, (i < a->n_states - 1 ? ", ":""));
        }
        printf("\n");
    }
    else {
        printf("Can't load states from the file\n");
        return NULL;
    }
    fscanf(f, "%s", temp); //Lê "delta"
    //Carrega as funcoes delta do automato
    if(!strcmp(temp, "delta")) {
        fscanf(f, "%s", temp); //Lê as funções delta
        if(!carrega_delta(&a, temp)) {
            printf("Can't load delta from the file\n");
            return NULL;
        }
        printf("%d functions loaded:\n", a->n_functions);
        int i;
        for(i = 0; i < a->n_functions; i++) {
            printf("(%s,%s,%s)\n", a->transition_functions[i].src.state_name, a->transition_functions[i].transition, a->transition_functions[i].dest.state_name );
        }
    }
    else {
        printf( "Can't find delta functions in the file\n");
        return NULL;
    }
    fscanf(f, "%s", temp); // Lê "inicial"
    //Carrega o estado inicial do automato
    if(!strcmp(temp, "inicial")) {
        fscanf(f, "%s", temp); // Lê o estado inicial
        if(!carrega_inicial(&a, temp)) {
          return NULL;
          printf("Nao foi possivel carregar o estado inicial\n");
        }
        if(!strlen(a->begin_state.state_name)) {
          printf("O automato deve ter um estado inicial\n");
          return NULL;
        }
        printf("Estado inicial carregado:\n" "%s\n", a->begin_state.state_name);
    }
    else {
        printf("Nao foi possivel encontrar o estado inicial do automato no arquivo\n");
        return NULL;
    }
    fscanf(f, "%s", temp); // Lê "final"
    //Carrega os estados final do Automato
    if(!strcmp(temp, "final")) {
        fscanf(f, "%s", temp); //Lê os estados finais
        printf("Leu a sessao estados finais do arquivo: %s\n", temp);
        if(!carrega_final(&a, temp))
            return NULL;
        if(a->n_final_states == 1)
          printf("Estado final:\n" "%s\n", a->final_states[0].state_name);
        else if(a->n_final_states <= 0){
          printf("O automato deve ter pelo menos um estado final\n");
          return NULL;
        }
        else {
          printf("%d estados finais carregados:\n", a->n_final_states);
          int i;
          for(i = 0; i < a->n_final_states; i++) {
            printf("%s%s", a->final_states[i].state_name, (i < a->n_final_states - 1 ? ", ":""));
          }
          printf("\n");
        }
    }
    else {
        printf("Nao foi possivel encontrar o estado final do automato\n");
        return NULL;
    }
    return a;
}

//Retorna 1 se sucesso, 0 se erro
int carrega_final(Automaton *a, char *final) {
    int i = 0, j = 0, k = 0, m = 0; //k: quantidade de estados finais
    char temp[16];
    //i percorre os estados do automato
    //j percore o vetor temp e o de estado final
    //k percorre o vetor de estados finais
    //m percorre o vetor final
    while(1) { //Percore todo o vetor de estados finais recebido
        if(final[m] != ',' && final[m] != '\0') { //Verifica se acabou a palavra lida
            if((*a)->states[i].state_name[j] == final[m]) {  //Verifica se cada caractere são iguais
                temp[j] = final[m];
                temp[j+1] = '\0';
                j++;
                m++;
            } else { //Se os dois estados forem diferentes, avança pro próximo estado
                if(i < (*a)->n_states) { //Verifica se os estados acabaram
                    i++;
                }
                else { //Se os estados tiverem acabado, retorna erro
                    printf("Total de estados em n_final_states %d\n",(*a)->n_final_states);
                    printf("Estado final invalido encontrado\n");
                    return 0;
                }
            }
        } else { //Se a palavra acabou, escreve temp em um estado final
            int n;
            for(n = 0; n < strlen(temp); n++) {
                (*a)->final_states[k].state_name[n] = temp[n];
            }
            temp[0] = '\0';
            k++;
            if(final[m] == '\0') break;
            m++;
            j = 0;
            i = 0;
        }
    }
    (*a)->n_final_states = k;
    return 1;
}

//Retorna 1 se sucesso, 0 se erro
int carrega_inicial(Automaton *a, char *inicial) {
    int i, j;
    unsigned n;
    for(i = 0; i < (*a)->n_states; i++) { //Verifica se a palavra existe nos estados do automato
        j = 0;
        while((*a)->states[i].state_name[j] != '\0' && inicial[j] != '\0') { //Equanto a palavra não acabar
            if(inicial[j+1] == ',') {
                printf("So e permitido um estado inicial para o automato\n");
                return 0;
            }
            if((*a)->states[i].state_name[j] != inicial[j])
                break;
            if(inicial[j+1] != '\0')
                j++;
            else {
                for(n = 0; n < strlen(inicial); n++) {
                    (*a)->begin_state.state_name[n] = inicial[n];
                }
                j++;
                break;
            }
        }
        if(j >= strlen(inicial))
            break;
    }
    if(!strlen((*a)->begin_state.state_name)) {
        printf("Nenhum estado inicial foi encontrado\n");
        return 0;
    }
    return 1;
}

//Retorna 1 se sucesso, 0 se erro
int carrega_delta(Automaton *a, char *delta) {
    if(delta[0] != '{') {
        printf("As funcoes devem estar entre chaves, no seguinte formato: {([estado origem],[caractere do alfabeto],[estado destino]),([estado origem],[caractere do alfabeto],[estado destino]),...}. Note que nao se usa espaco entre cada funcao\n");
        return 0;
    }
    int j = 1, k, n = 0;
    //j: percorre delta
    //k: percorre o vetor temp
    while(delta[j] != '\0') { //Percorre todo o vetor delta
        if(delta[j] == '(') {
            while(delta[j] != ')') { //Percorre cada tupla
                if(delta[j] != '(') {
                    printf("Formato invalido das funcoes no arquivo\n");
                    return 0;
                }
                else
                    j++;
                char temp[15];
                k = 0;
                while(delta[j] != ',') {
                    temp[k] = delta[j];
                    j++;
                    k++;
                }
                temp[k]='\0';
                if(!pertence_estado(*a, temp)) {
                    printf("Estado 1: O estado nao %s pertence ao automato\n", temp);
                    return 0;
                }
                atribui((*a)->transition_functions[n].src.state_name, temp);
                j++;
                // if(!pertence_alfabeto(*a, delta[j])) {
                //     printf("O simbolo de transicao deve fazer parte do alfabeto do automato, \"%c\" nao pertence ao alfabeto\n", delta[j]);
                //     return 0;
                // }
                //TODO : A transicao n eh mais um caracter individual, tem que refazer 
                (*a)->transition_functions[n].transition[0] = delta[j];
                if(delta[j+1] != ',') {
                    printf("O simbolo de transicao deve ser apenas um caractere\n");
                    return 0;
                }
                j+=2;
                k = 0;

                while(delta[j] != ')') {
                    temp[k] = delta[j];
                    j++;
                    k++;
                }
                temp[k]='\0';
                if(!pertence_estado(*a, temp)) {
                    printf("Estado 2: O estado %s nao pertence ao automato\n", temp);
                    return 0;
                }

                atribui((*a)->transition_functions[n].dest.state_name, temp);

                n++;
            }
        }
        else {
            j++;
        }
    }
    if(delta[j-1] != '}') {
        printf("As funcoes devem estar entre chaves no arquivo\n");
        return 0;
    }
    (*a)->n_functions = n;
    return 1;
}

//Verifica se o estado pertence ao automato
int pertence_estado(Automaton a, char *estado) {
    unsigned i;
    for(i = 0; i < a->n_states; i++) {
        if(!strcmp(a->states[i].state_name, estado))
            return 1;
    }
    return 0;
}

void carrega_estados(Automaton *a, char *estados) {
    int i, j = 0, k = 0;
    char temp[40];
    char temp2[40];
    char truple_values[4][40];
    int n_elements_in_truple=0;
    int is_truple = 0;

    //Percorre cada caractere da string de estados
    for(i = 0; i <= strlen(estados); i++) {
        //Se for um objeto 
        if(estados[i] == '(' || estados[i] == ')'){
            if(estados[i] == '(')
                is_truple=1;
            else
                is_truple=0;     
        }
        else{
            if((estados[i] != ',' || is_truple) && estados[i] != '\0') {
                temp[k] = estados[i];
                temp[k+1] = '\0';
                k++;
            } else {
                k = 0;
                int n;
                //for(n = 0; n <= strlen(temp); n++)
                //    (*a)->states[j].state_name[n] = temp[n];
                int i_temp2=0;
                n_elements_in_truple=0;
                for(int i=0; i<strlen(temp)+1; i++){
                    if(temp[i] != ',' && temp[i] != '\0'){
                        temp2[i_temp2] = temp[i];
                        i_temp2++;
                    }
                    else{
                        temp2[i_temp2] = '\0';
                        strcpy(truple_values[n_elements_in_truple], temp2);
                        n_elements_in_truple++;
                        temp2[0] = '\0';
                        i_temp2=0;
                    }
                }
                if(n_elements_in_truple==1){
                    inserirEstado(a, truple_values[0],truple_values[1], "", 0);
                }
                else{
                    if(truple_values[3][0] == 't')
                        inserirEstado(a, truple_values[0],truple_values[1], truple_values[2], 1);
                    else
                        inserirEstado(a, truple_values[0],truple_values[1], truple_values[2], 0);
                }
                j++;
            }
        }
    }
    (*a)->n_states = j;
}

State *inserirEstado(Automaton *a, char *state_name, char *token_type, char *token_name, int return_car){
    State *s = (State*) malloc(sizeof(struct state));
    if(s == NULL)
        return NULL;
    strcpy(s->state_name, state_name);
    s->token_type = getIntTokenType(token_type);
    strcpy(s->token_name, token_name);
    s->return_car = return_car;
    (*a)->states[(*a)->n_states] = *s;
    (*a)->n_states++;
    return s;
}

int getIntTokenType(char *tokenType) {
    char token_upper[40];
    for(int i=0; i<strlen(tokenType); i++)
        token_upper[i] = toupper(tokenType[i]);
    token_upper[strlen(tokenType)] = '\0';
    if(tokenType == NULL)
        return -1;
    if(!strcmp("PROG", token_upper))
        return PROG;
    if(!strcmp("ID", token_upper))
        return ID;
    if(!strcmp("OCURL", token_upper))
        return OCURL;
    if(!strcmp("CCURL", token_upper))
        return CCURL;
    if(!strcmp("IF", token_upper))
        return IF;
    if(!strcmp("THEN", token_upper))
        return THEN;
    if(!strcmp("ELSE", token_upper))
        return ELSE;
    if(!strcmp("RELOP", token_upper))
        return RELOP;
    if(!strcmp("TYPE", token_upper))
        return TYPE;
    if(!strcmp("OP", token_upper))
        return OP;
    if(!strcmp("OPAR", token_upper))
        return OPAR;
    if(!strcmp("CPAR", token_upper))
        return CPAR;
    if(!strcmp("COLON", token_upper))
        return COLON;
    if(!strcmp("WHILE", token_upper))
        return WHILE;
    if(!strcmp("DO", token_upper))
        return DO;
    if(!strcmp("CONST", token_upper))
        return CONST;
    if(!strcmp("COMMT", token_upper))
        return COMMT;
    return -1;
}

//Atribui o valor de b em a
void atribui(char *a, char *b) {
    unsigned i;
    for(i = 0; i < strlen(b); i++) {
        a[i] = b[i];
        a[i+1] = '\0';
    }
}


void mostrarAutomato(Automaton a){
    int i;
    printf("Estados:\n{");
    for(i=0;i<a->n_states;i++)
        printf("%s%s",a->states[i].state_name, i < a->n_states - 1 ? ", " : "}\n");
    printf("Número de estados: %d\n",a->n_states);
    //printf("Alfabeto:\n%s\n",a->alfabeto);
    printf("Funções de transição\n");
    for(i=0;i<a->n_functions;i++)
        printf("Origem: %s\t Destino: %s\t Transição: %s\n",a->transition_functions[i].src.state_name,a->transition_functions[i].dest.state_name,a->transition_functions[i].transition);
    printf("Quantidade de funções de transição: %d\n",a->n_functions);
    printf("Estado inicial: \"%s\"\n",a->begin_state.state_name);
    if(a->n_final_states > 1)
      printf("Estados finais: ");
    else
      printf("Estado final: ");
    for(i=0;i<a->n_final_states;i++)
        printf("\"%s\"\n",a->final_states[i].state_name);
}

void mostrarEstados(Automaton a){
    int i;
    printf("Estados:\n");
    for(i=0;i<a->n_states;i++){
       printf("State Name: %s\n", a->states[i].state_name);
       printf("Token type: %d\n", a->states[i].token_type);
       printf("Token Name: %s\n", a->states[i].token_name);
       printf("Return Car: %d\n\n", a->states[i].return_car);
    }
    printf("Número de estados: %d\n",a->n_states);
}

// ReturnState consume(Automaton a, char c) {
//     //exemplo delta 0,[a-z],1
//     Delta relevant_deltas[300]; // estados que possuem o estado inicial como state0
//     ReturnState return_state = { -1, 0};
//     for (int i = 0; i < a->n_functions; i++) {
//         if (match(c, relevant_deltas[i])) {
//             move_state(relevant_deltas[i].dest);
//             if(final_state(relevant_deltas[i])) {
//                 return relevant_deltas[i]->dest->token;
//             }
//         }
//     }
//     return ;
// }