#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <sys/time.h>
#include "Autocompletador.h"

pNodo* build_dictionary(char *dictionary_name)
{
    // returns 0 if no error occurred
    int dic_size, aux, counter;
    // dic_size is the number of words in the dictionary, defined in the first line
    char line[max_size_value+max_size_word+2]; //weight+space+word+'\n'
    FILE* dic_file;
    pNodo* dictionary;
    dictionary = new_node(0);
    if((dic_file = fopen(dictionary_name, "r")) == NULL)
    {
        printf("\nERROR: could not open the dictionary file!\n\n");
        return NULL;
    }
    dic_size = 0;
    while(((aux = fgetc(dic_file)) != '\n')&&(aux != EOF))
    {
        dic_size = 10*dic_size + (aux-'0');
    }

    counter = 0;
    while(fgets(line, (max_size_value+max_size_word+2)*sizeof(char), dic_file) != NULL)
    {
        counter += add_word(dictionary, line);
    }
    if(counter != 0)
    {
        printf("ERROR: %i words were not added in the dictionary due to undefined symbols!", counter);
    }

    /*******************************************************/

    if(fclose(dic_file)!=0)
    {
        printf("\nERROR: could not close the dictionary file!\n\n");
        return NULL;
    }
    return dictionary;
}

pNodo* new_node(char symbol)
{
    // allocates the node, makes all it's children point to NULL, and defines value and value_children as 0
    pNodo* node;
    node = (pNodo*) malloc(sizeof(pNodo));
    int i;
    for(i=0; i<number_of_symbols; i++)
    {
        node->children[i] = NULL;
    }
    node->symbol = symbol;
    node->value = 0;
    node->value_children = 0;
    return node;
}

int index_children(char symbol){
    // returns -1 if receives an undefined symbol
    symbol = tolower(symbol);
    if(symbol >= 'a' && symbol <= 'z')
        return symbol - 'a';
    else if(symbol >= '0' && symbol<= '9')
        return symbol - '0' +26; // 26 is the number of letters in the alphabet
    else if(symbol == '\'')
        return 36; // letters + digits
    else if(symbol == '.')
        return 37;
    else if(symbol == '-')
        return 38;
    return -1;
}

int add_word(pNodo *dictionary, char *line)
{
    // returns 0 if no error occurred
    /*** THE WORDS ARE NOT BEING ADDED CORRECTLY ***/
    long long int word_value;
    int i, line_size, index;
    char symbol;
    pNodo *aux_pointer;
    word_value = 0;
    for(i=0; i<max_size_value; i++)
    {
        if(line[i]>='0' && line[i]<='9')
        {
            word_value = 10*word_value + line[i]-'0';
        }
    }
    aux_pointer = dictionary;
    if(aux_pointer->value_children < word_value)
        aux_pointer->value_children = word_value;
    line_size = strlen(line);
    for(i=max_size_value+1; i<(line_size-2); i++)
    {
        // the last line[i] element is treated differently due to value and value_children
        // it's subtracted 2 because of the '\n' in the end of the line
        symbol = tolower(line[i]);
        if((index = index_children(symbol)) >= 0){
            if(aux_pointer->children[index] == NULL)
            {
                // if the node doesn't exist, it's created
                aux_pointer->children[index] = new_node(symbol);
            }
            aux_pointer = aux_pointer->children[index];
            if(aux_pointer->value_children < word_value)
                aux_pointer->value_children = word_value;
        }
        else
        {
            //printf("\nERROR: undefined symbol in the dictionary file!\n\n");
            //getchar();
            return 1;
        }
    }
    symbol = tolower(line[i]);
    if((index = index_children(symbol)) >= 0)
    {
        if(aux_pointer->children[index] == NULL)
        {
            // if the node doesn't exist, it's created
            aux_pointer->children[index] = new_node(symbol);
        }
        aux_pointer = aux_pointer->children[index];
        aux_pointer->value = word_value;
    }
    else
    {
        //printf("\nERROR: undefined symbol in the dictionary file!\n\n");
        //getchar();
        return 1;
    }
    return 0;
}


int auto_complete(pNodo *dictionary, char *input_file, char *output_file, int number_of_suggestions){
    struct timeval t1, t2;
    double elapsed_time = 0;
    int i, j, suggestions_made, suggestions_complete;
    FILE *input, *output;
    pNodo *current_node;
    char current_word[255];
    Suggestion suggestions[number_of_suggestions];

    if((input = fopen(input_file, "r")) == NULL)
    {
        printf("\nERROR: could not open the input file!\n\n");
        return 1;
    }
    if((output = fopen(output_file, "w+")) == NULL)
    {
        printf("\nERROR: could not open the output file!\n\n");
        return 1;
    }
    while(fscanf(input, "%s", current_word) != EOF){
        gettimeofday(&t1, NULL); // starts time count for this word
        fprintf(output, "%s\n", current_word);
        current_node = dictionary;
        for(i = 0; i < number_of_suggestions ; i++){
            suggestions[i].value = 0;
            strcpy(suggestions[i].word, "");
        }
        i = 0;
        while(current_word[i] != '\0' && current_node != NULL){
            j = index_children(current_word[i]);
            current_node = current_node->children[j];
            i += 1;
        }
        if(current_node == NULL){
            fprintf(output, "\tnenhum resultado encontrado\n");
        } else {
            make_suggestions(current_word, current_node, suggestions, number_of_suggestions);
            for(j=0;j<number_of_suggestions;j++){
                if(suggestions[j].value > 0){
                    fprintf(output, "%14d %s\n", suggestions[j].value, suggestions[j].word);
                }
            }
        }
        gettimeofday(&t2, NULL); // ends time counting for this word
        elapsed_time += (t2.tv_sec - t1.tv_sec) * 1000.0;
        elapsed_time += (t2.tv_usec - t1.tv_usec) / 1000.0;
    }

    fprintf(output, "\nTempo: %fms", elapsed_time);
    fclose(input);
    fclose(output);
    return 0;
}

void make_suggestions(char *current_string, pNodo *current_node, Suggestion * current_suggestions, int number_of_suggestions){
    if(current_node == NULL)
        return;
    int i, value;
    char letter[2], new_current_string[max_size_word];
    Suggestion s;

    value = current_node->value;
    if(value != 0){
        s.value = value;
        strcpy(s.word, current_string);
        add_suggestion(current_suggestions, s, number_of_suggestions);
    }
    if(current_node->value_children > current_suggestions[number_of_suggestions - 1].value){
        for(i = 0; i < number_of_symbols; i++){
            if(current_node->children[i] != NULL){
                sprintf(letter, "%c", current_node->children[i]->symbol);
                strcpy(new_current_string, current_string);
                strcat(new_current_string, letter);
                make_suggestions(new_current_string, current_node->children[i], current_suggestions, number_of_suggestions);
            }
        }
    }
}

void add_suggestion(Suggestion * suggestions, Suggestion new_suggestion, int number_of_suggestions){
    int inserted = 0, i = 0, j;
    while(inserted == 0 && i < number_of_suggestions){
        if(suggestions[i].value < new_suggestion.value){
            for(j = number_of_suggestions - 1; j > i; j--){
                suggestions[j] = suggestions[j-1];
            }
            suggestions[i] = new_suggestion;
            inserted = 1;
        }
        i++;
    }
}
