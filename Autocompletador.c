#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "Autocompletador.h"

int build_dictionary(char *dictionary_name)
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
        getchar();
        return 1;
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
        getchar();
    }

    /*******************************************************/

    if(fclose(dic_file)!=0)
    {
        printf("\nERROR: could not close the dictionary file!\n\n");
        getchar();
        return 1;
    }
    return 0;
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
