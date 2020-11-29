#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char* argv[]) {
    char source_code[128];
    char automaton[128];
    if (argc != 3) {
        printf("Usage: <application name> <source code path> <automaton path>\n");
    }
    strncpy(source_code, argv[1], 128);
    strncpy(automaton, argv[2], 128);
    printf("Parsing %s source code using %s automaton\n", source_code, automaton);

    return EXIT_SUCCESS;
}
