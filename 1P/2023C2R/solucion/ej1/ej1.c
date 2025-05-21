#include "ej1.h"

string_proc_list* string_proc_list_create(void){
	string_proc_list* lista = malloc(sizeof(string_proc_list));
	lista->first = NULL;
	lista->last = NULL;
}

string_proc_node* string_proc_node_create(uint8_t type, char* hash){
	string_proc_node* nodo = malloc(sizeof(string_proc_node));
	nodo->next = NULL;
	nodo->previous = NULL;
	nodo->type = type;
	nodo->hash = hash;
	return nodo;
}

void string_proc_list_add_node(string_proc_list* list, uint8_t type, char* hash){
	string_proc_node* nodo = string_proc_node_create(type, hash);
	if (list->first == NULL) { // o sea, si es vacía
		list->first = nodo;
		list->last = nodo;
	} else {
		nodo->previous = list->last;
		list->last->next = nodo;
		list->last = nodo;
	}
}

char* string_proc_list_concat(string_proc_list* list, uint8_t type , char* hash){
    char* result = NULL;
    string_proc_node* current = list->first;

    // Concatenar los hashes de los nodos con el type pedido
    while (current != NULL) {
        if (current->type == type) {
            if (result == NULL) {
                // Primer hash: hacer una copia
                result = str_concat("", current->hash);
            } else {
                char* temp = str_concat(result, current->hash);
                free(result);
                result = temp;
            }
        }
        current = current->next;
    }

    // Concatenar el hash recibido como parámetro al final
    if (result == NULL) {
        // Si no hubo ningún nodo con ese type, solo devolver copia de hash
        result = str_concat("", hash);
    } else {
        char* temp = str_concat(result, hash);
        free(result);
        result = temp;
    }

    return result;
}


/** AUX FUNCTIONS **/

void string_proc_list_destroy(string_proc_list* list){

	/* borro los nodos: */
	string_proc_node* current_node	= list->first;
	string_proc_node* next_node		= NULL;
	while(current_node != NULL){
		next_node = current_node->next;
		string_proc_node_destroy(current_node);
		current_node	= next_node;
	}
	/*borro la lista:*/
	list->first = NULL;
	list->last  = NULL;
	free(list);
}
void string_proc_node_destroy(string_proc_node* node){
	node->next      = NULL;
	node->previous	= NULL;
	node->hash		= NULL;
	node->type      = 0;			
	free(node);
}


char* str_concat(char* a, char* b) {
	int len1 = strlen(a);
    int len2 = strlen(b);
	int totalLength = len1 + len2;
    char *result = (char *)malloc(totalLength + 1); 
    strcpy(result, a);
    strcat(result, b);
    return result;  
}

void string_proc_list_print(string_proc_list* list, FILE* file){
        uint32_t length = 0;
        string_proc_node* current_node  = list->first;
        while(current_node != NULL){
                length++;
                current_node = current_node->next;
        }
        fprintf( file, "List length: %d\n", length );
		current_node    = list->first;
        while(current_node != NULL){
                fprintf(file, "\tnode hash: %s | type: %d\n", current_node->hash, current_node->type);
                current_node = current_node->next;
        }
}