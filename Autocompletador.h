#define number_of_symbols   39
#define max_size_word       30
#define max_size_value      14

/*
a to z = 26
0 to 9 = 10
' . -  =  3
total  = 39
*/
struct PtNodo
{
    char symbol;
    long long int value;  // it's the value of the word that ends in this node
    long long int value_children; // it's the value of the most valuable children it has
    struct PtNodo* children[number_of_symbols];
};
typedef struct PtNodo pNodo;

struct suggestion {
    int value;
    char word[max_size_word];
};
typedef struct suggestion Suggestion;

pNodo* build_dictionary(char *dictionary_name);
pNodo* new_node(char symbol);
int index_children(char symbol);
int add_word(pNodo *dictionary, char *line);

int auto_complete(pNodo *dictionary, char *input_file, char *output_file, int number_of_suggestions);
void make_suggestions(char *current_string, pNodo *current_node, Suggestion * current_suggestions, int number_of_suggestions);
void add_suggestion(Suggestion * suggestions, Suggestion new_suggestion, int number_of_suggestions);
