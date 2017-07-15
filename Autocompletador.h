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

int build_dictionary(char *dictionary_name);
pNodo* new_node(char symbol);
int index_children(char symbol);
int add_word(pNodo *dictionary, char *line);
