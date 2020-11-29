#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

int carrega_final(Automaton *a, char *final);
int carrega_inicial(Automaton *a, char *inicial);
void carrega_estados(Automaton *a, char *estados);
int carrega_delta(Automaton *a, char *delta);
State *pertence_estado(Automaton a, char *estado);
void atribui(char *a, char *b);
void mostrarAutomato(Automaton a);


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
    return 0;
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
    int j = 1, k, current_delta = 0;
    //j: percorre delta
    //k: percorre o vetor temp
    while(delta[j] != '\0') { //Percorre todo o vetor delta
        if(delta[j] == '(') {
            while(delta[j] != ')') { //Percorre cada tupla
                if(delta[j] != '(') {
                    printf("Formato invalido das funcoes no arquivo\n");
                    return 0;
                }
                else {
                    j++;
                }
                char temp_delta[15];
                k = 0;
                while(delta[j] != ',') {
                    temp_delta[k] = delta[j];
                    j++;
                    k++;
                }
                temp_delta[k]='\0';

                printf("Verificando se o estado lido pertence aos estados do automato\n");
                State* src = pertence_estado(*a, temp_delta);
                if(src == NULL) {
                    printf("Estado 1: O estado nao %s pertence ao automato\n", temp_delta);
                    return 0;
                }
                printf("Pertence estado funcionando.\n");
                (*a)->transition_functions[current_delta].src = *src;
                // atribui((*a)->transition_functions[n].src.state_name, temp); // < esse é o que tava
                // atribui(src->state_name, temp); // < esse foi o que eu modifiquei
                
                j++;
                // if(!pertence_alfabeto(*a, delta[j])) {
                //     printf("O simbolo de transicao deve fazer parte do alfabeto do automato, \"%c\" nao pertence ao alfabeto\n", delta[j]);
                //     return 0;
                // }
                //TODO : A transicao n eh mais um caracter individual, tem que refazer 
                if (delta[j] != ',') {
                    printf("Expected ',' at delta column %d\n", j);
                    exit(1);
                }
                j++;
                if (delta[j] != '"') {
                    printf("Expected '\"' at delta column %d\n", j);
                    exit(1);
                }
                j++;
                char temp_regex[15];
                k = 0;
                while(delta[j] != '"') {
                    if (k >= 14) {
                        printf("Max regex size reached at %d\n", j);
                        exit(1);
                    }
                    temp_regex[k++] = delta[j++];
                }
                temp_regex[j] = '\0';
                strncpy((*a)->transition_functions[current_delta].transition, temp_regex, 15);
                j++;
                if (delta[j] != ',') {
                    printf("Expected ',' at %d\n", j);
                    exit(1);
                }
                j++;

                k = 0;
                while(delta[j] != ')') {
                    temp_delta[k] = delta[j];
                    j++;
                    k++;
                }
                temp_delta[k]='\0';
                State *dest = pertence_estado(*a, temp_delta);
                if(dest != NULL) {
                    printf("Estado 2: O estado %s nao pertence ao automato\n", temp_delta);
                    return 0;
                }

                (*a)->transition_functions[current_delta].dest = *dest;
                // atribui((*a)->transition_functions[current_delta].dest.state_name, temp_delta);

                current_delta++;
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
    (*a)->n_functions = current_delta;
    return 1;
}

//Verifica se o estado pertence ao automato
State *pertence_estado(Automaton a, char *estado) {
    unsigned i;
    for(i = 0; i < a->n_states; i++) {
        printf("%s\n", a->states[i].state_name);
        if(!strcmp(a->states[i].state_name, estado)) {
            return &(a->states[i]);
        }
    }
    return NULL;
}

void carrega_estados(Automaton *a, char *estados) {
    int i, j = 0, k = 0;
    char temp[40];
    char temp2[40];
    char truple_values[4][40];
    int n_elements_in_truple=0;
    int is_truple = 0;
    for(i = 0; i <= strlen(estados); i++) {
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
                printf("Leu o estado %s\n", temp);
                int i_temp2=0;
                n_elements_in_truple=0;
                for(int i=0; i<strlen(temp); i++){
                    if(temp[i] != '\0' && temp[i] != ','){
                        temp2[i_temp2] = temp[i];
                        i_temp2++;
                    }
                    else{
                        temp2[i_temp2] = '\0';
                        //printf("Pegou %s\n", temp2);
                        strcpy(truple_values[n_elements_in_truple], temp2);
                        n_elements_in_truple++;
                        i_temp2=0;
                        temp2[0] = '\0';
                    }
                    //printf("Pegou %s\n", truple_values[n_elements_in_truple]);
                }
                j++;
            }
        }
    }
    (*a)->n_states = j;
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