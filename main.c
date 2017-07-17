#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Autocompletador.h"

int main(int argc, char *argv[])
{
    pNodo * dictionary;

    if(argc != 5){
        printf("Invalid application parameters.\n");
        printf("./main dictionary input output numberOfSuggestions\n");
        exit(1);
    }
    char * dictionary_file = argv[1];
    char * input_file = argv[2];
    char * output_file = argv[3];
    int number_of_suggestions = atoi(argv[4]);

    dictionary = build_dictionary(dictionary_file);
    if(dictionary == NULL){
        printf("Empty dictionary.\n");
        exit(1);
    }
    auto_complete(dictionary, input_file, output_file, number_of_suggestions);
    return 0;
}
