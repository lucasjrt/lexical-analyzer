#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <regex.h>

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
    char state_name[32];
    int token_type;
    char token_name[32];
    int return_car;
};
typedef struct state State;

struct delta {
    State src;
    State dest;
    char transition[3500];
};
typedef struct delta Delta;


struct automaton {
    State states[300];
    int n_states;
    Delta transition_functions[300];
    int n_functions;
    State *begin_state;
    State *current_state;
};

int load_initial(Automaton *a, char *initial);
void load_states(Automaton *a, char *states);
int load_deltas(Automaton *a, char *delta);
State *get_state(Automaton a, char *state);
void show_automaton(Automaton a);
State *insert_state(Automaton *a, char *state_name, char *token_type, char *token_name, int return_car);
void show_states(Automaton a);
int get_int_token_type(char *tokenType);
ReturnState consume(Automaton a, char c);
int match_regex(char *string, char ch);

Automaton create_automaton(char* path) {
    FILE *f = fopen(path, "r");
    if(f == NULL) {
      printf("File not found\n");
      return NULL;
    }
    Automaton a = (Automaton) malloc(sizeof(struct automaton));
    char temp[3500];
    //Loads automaton states
    fscanf(f, "%s", temp); //Read "states"
    if(!strcmp(temp, "states")) {
        fscanf(f, "%s", temp); //Read the states
        load_states(&a, temp);
    }
    else {
        printf("Can't load states from the file\n");
        return NULL;
    }
    fscanf(f, "%s", temp); //Read "delta"
    //Load delta functions from automaton
    if(!strcmp(temp, "delta")) {
        fscanf(f, "%s", temp); //Read delta functions
        if(!load_deltas(&a, temp)) {
            printf("Can't load delta from the file\n");
            return NULL;
        }
    }
    else {
        printf( "Can't find delta functions in the file\n");
        return NULL;
    }
    fscanf(f, "%s", temp); // Read "initial"
    //Load the automaton initial state
    if(!strcmp(temp, "initial")) {
        fscanf(f, "%s", temp); // Read the initial state
        if(!load_initial(&a, temp)) {
          return NULL;
          printf("Could not load initial state\n");
        }
        if(!strlen(a->begin_state->state_name)) {
          printf("The automaton must have an initial state\n");
          return NULL;
        }
        a->current_state = a->begin_state;
    }
    else {
        printf("Could not find the automaton initial state\n");
        return NULL;
    }
    return a;
}

//Returna 1 if success, 0 if error
int load_initial(Automaton *a, char *initial) {
    int i, j;
    for(i = 0; i < (*a)->n_states; i++) { //Verify if the word exists in the automaton states
        j = 0;
        while((*a)->states[i].state_name[j] != '\0' && initial[j] != '\0') { //While the words are not done
            if(initial[j+1] == ',') {
                printf("Just one initial state is allowed to the automaton\n");
                return 0;
            }
            if((*a)->states[i].state_name[j] != initial[j])
                break;
            if(initial[j+1] != '\0')
                j++;
            else {
                State *begin_state = get_state(*a, initial);
                if(begin_state == NULL){
                    printf("Invalid begin state\n");
                    exit(1);
                }
                (*a)->begin_state = begin_state;
                j++;
                break;
            }
        }
        if(j >= strlen(initial))
            break;
    }
    if(!strlen((*a)->begin_state->state_name)) {
        printf("No initial state was found\n");
        return 0;
    }
    return 1;
}

//Retorn 1 if success, 0 if error
int load_deltas(Automaton *a, char *delta) {
    int j = 0, k, current_delta = 0;
    //j: go through delta
    //k: go through temp vector
    while(delta[j] != '\0') { //Go through the whole delta vector
        if(delta[j] == '(') {
            while(delta[j] != ')') { //Go through each tuple
                if(delta[j] != '(') {
                    printf("Invalid function format in the file\n");
                    return 0;
                }
                else {
                    j++;
                }
                char temp_src[15];
                k = 0;
                while(delta[j] != ',') {
                    if(k >= 14) {
                        printf("Max state name reached for src state. Column: %d\n", j);
                        exit(1);
                    }
                    temp_src[k] = delta[j];
                    j++;
                    k++;
                }
                temp_src[k]='\0';

                State* src = get_state(*a, temp_src);
                if(src == NULL) {
                    printf("State src: The state %s does not belong to the automaton\n", temp_src);
                    return 0;
                }
                (*a)->transition_functions[current_delta].src = *src;
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
                char temp_regex[3500];
                k = 0;
                while(delta[j] != '"') {
                    if (k >= 3500) {
                        printf("Max regex size reached at %d\n", j);
                        exit(1);
                    }
                    temp_regex[k++] = delta[j++];
                }
                temp_regex[k] = '\0';

                strncpy((*a)->transition_functions[current_delta].transition, temp_regex, 3500);
                j++;
                if (delta[j] != ',') {
                    printf("Expected ',' at %d\n", j);
                    exit(1);
                }
                j++;

                k = 0;
                while(delta[j] != ')') {
                    temp_src[k] = delta[j];
                    j++;
                    k++;
                }
                temp_src[k]='\0';
                State *dest = get_state(*a, temp_src);
                if(dest == NULL) {
                    printf("State dest: The state %s does not belong to the automaton\n", temp_src);
                    return 0;
                }

                (*a)->transition_functions[current_delta].dest = *dest;
                current_delta++;
            }
        }
        else {
            j++;
        }
    }
    (*a)->n_functions = current_delta;
    return 1;
}

//Get a state from the automaton
State *get_state(Automaton a, char *estado) {
    unsigned i;
    for(i = 0; i < a->n_states; i++) {
        if(!strcmp(a->states[i].state_name, estado)) {
            return &(a->states[i]);
        }
    }
    return NULL;
}

void load_states(Automaton *a, char *states) {
    int i, j = 0, k = 0;
    char temp[40];
    char temp2[40];
    char tuple_values[4][40];
    int n_elements_in_tuple=0;
    int is_tuple = 0;

    //Goes through each character in a string of states
    for(i = 0; i <= strlen(states); i++) {
        //If it is an object
        if(states[i] == '(' || states[i] == ')'){
            if(states[i] == '(')
                is_tuple=1;
            else
                is_tuple=0;     
        }
        else{
            if((states[i] != ',' || is_tuple) && states[i] != '\0') {
                temp[k] = states[i];
                temp[k+1] = '\0';
                k++;
            } else {
                k = 0;
                int i_temp2=0;
                n_elements_in_tuple=0;
                for(int i=0; i<strlen(temp)+1; i++){
                    if(temp[i] != ',' && temp[i] != '\0'){
                        temp2[i_temp2] = temp[i];
                        i_temp2++;
                    }
                    else{
                        temp2[i_temp2] = '\0';
                        strcpy(tuple_values[n_elements_in_tuple], temp2);
                        n_elements_in_tuple++;
                        temp2[0] = '\0';
                        i_temp2=0;
                    }
                }
                if(n_elements_in_tuple==1){
                    insert_state(a, tuple_values[0], NULL, "", 0);
                }
                else{
                    if(tuple_values[3][0] == 't')
                        insert_state(a, tuple_values[0],tuple_values[1], tuple_values[2], 1);
                    else
                        insert_state(a, tuple_values[0],tuple_values[1], tuple_values[2], 0);
                }
                j++;
            }
        }
    }
    (*a)->n_states = j;
}

State *insert_state(Automaton *a, char *state_name, char *token_type, char *token_name, int return_car){
    State *s = (State*) malloc(sizeof(struct state));
    if(s == NULL)
        return NULL;
    strcpy(s->state_name, state_name);
    if(token_type == NULL){
        s->token_type = -1;
    }else{
        s->token_type = get_int_token_type(token_type);
    }
    strcpy(s->token_name, token_name);
    s->return_car = return_car;
    (*a)->states[(*a)->n_states] = *s;
    (*a)->n_states++;
    return s;
}

int get_int_token_type(char *tokenType) {
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

void show_automaton(Automaton a){
    int i;
    printf("States:\n{");
    for(i=0;i<a->n_states;i++)
        printf("%s%s",a->states[i].state_name, i < a->n_states - 1 ? ", " : "}\n");
    printf("Number of states: %d\n",a->n_states);
    printf("Transition functions\n");
    for(i=0;i<a->n_functions;i++)
        printf("Source: %s\t Destination: %s\t Transition: %s\n",a->transition_functions[i].src.state_name,a->transition_functions[i].dest.state_name,a->transition_functions[i].transition);
    printf("Amount of transition functions: %d\n",a->n_functions);
    printf("Initial states: \"%s\"\n",a->begin_state->state_name);
}

void show_states(Automaton a){
    int i;
    printf("States:\n");
    for(i=0;i<a->n_states;i++){
       printf("State Name: %s\n", a->states[i].state_name);
       printf("Token type: %d\n", a->states[i].token_type);
       printf("Token Name: %s\n", a->states[i].token_name);
       printf("Return Car: %d\n\n", a->states[i].return_car);
    }
    printf("Number of states: %d\n",a->n_states);
}

int match_regex(char *string, char ch){      
    regex_t regex;
    int reti;
    char msgbuf[100];
    char string_to_match[2] = {ch, '\0'};

    reti = regcomp(&regex, string, 0);
    if (reti) {
        fprintf(stderr, "Could not compile regex\n");
        exit(1);
    }

    reti = regexec(&regex, string_to_match, 0, NULL, 0);
    
    if (!reti) {
        regfree(&regex);
        return 1;
    }
    else if (reti == REG_NOMATCH) {
        regfree(&regex);
        return 0;
    }
    else {
        regerror(reti, &regex, msgbuf, sizeof(msgbuf));
        fprintf(stderr, "Regex match failed: %s\n", msgbuf);
        exit(1);
    }
}

ReturnState consume(Automaton a, char c) {
    for (int i = 0; i < a->n_functions; i++) {
        // Transitions that have the current state in src state
        if(!strcmp(a->transition_functions[i].src.state_name, a->current_state->state_name)){
            if(match_regex(a->transition_functions[i].transition, c)){
                ReturnState return_structure;
                return_structure = (ReturnState) malloc(sizeof(struct returnState));
                strcpy(return_structure->token_name, a->transition_functions[i].dest.token_name);
                return_structure->tokenType = a->transition_functions[i].dest.token_type;
                return_structure->returnCar = a->transition_functions[i].dest.return_car;
                a->current_state = &(a->transition_functions[i].dest);
                return return_structure;
            }
        }
    }
    return NULL;
}

int reset_automaton(Automaton *a){
    (*a)->current_state = (*a)->begin_state;
    return 0;
}
