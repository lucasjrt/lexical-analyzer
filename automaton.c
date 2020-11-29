#include <stdio.h>
#include <stdlib.h>
#include "automaton.h"


int carrega_final(Automaton *a, char *final);
int carrega_inicial(Automaton *a, char *inicial);
int carrega_delta(Automaton *a, char *delta);


struct state{
    char state_name[15];
    int token_type;
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
    carrega_automato("Automato05.dat");
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
            printf("(%s,%s,%s)\n", a->transition_functions[i].src, a->transition_functions[i].transition, a->transition_functions[i].dest );
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
                if(i < (*a)->n_final_states) { //Verifica se os estados acabaram
                    i++;
                }
                    else { //Se os estados tiverem acabado, retorna erro
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
                atribui((*a)->transition_functions[n].src, temp);
                j++;
                if(!pertence_alfabeto(*a, delta[j])) {
                    printf("O simbolo de transicao deve fazer parte do alfabeto do automato, \"%c\" nao pertence ao alfabeto\n", delta[j]);
                    return 0;
                }
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

                atribui((*a)->transition_functions[n].dest, temp);

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