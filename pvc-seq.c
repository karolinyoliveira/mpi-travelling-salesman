#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

// Grafo orientado com matriz de adjacências
typedef struct _graph_t {
    int order;
    int **weights;
} graph_t;


// Desaloca um grafo
void free_graph(graph_t *graph){
    int i, order=graph->order;
    for(i=0; i<order; ++i)
        free(graph->weights[i]);
    free(graph->weights);
    free(graph);
}


// Cria um grafo de forma pseualeatória
graph_t *generate_graph(int order, int seed) {

    // Validação da ordem
    if(order < 1){
        printf("order must be greater than zero\n");
        return NULL;
    }

    // Criação do grafo
    graph_t *graph = (graph_t *) malloc(sizeof(graph_t));
    if(graph == NULL) {
        printf("failed to malloc graph\n");
        return NULL;   
    }

    // Aplica a semente
    srand(seed);

    // Criação dos pesos
    int **weights = (int **) malloc(order * sizeof(int *));
    int i, j;
    float random;
    for(i=0; i<order; ++i){
        weights[i] = (int *) malloc(order * sizeof(int));
        for(j=0; j<order; ++j){
            weights[i][j] = rand();
        }
    }

    // Impede retorno ao próprio nó
    for(i=0; i<order; ++i)
        weights[i][i] = 0;

    // Atribuição e retorno
    graph->order = order;
    graph->weights = weights;
    return graph;
}


// Gera uma lista de inteiros com o tamanho fornecido
int *generate_list(int length) {
    if(length < 1){
        printf("length must be greater than zero\n");
        return NULL;
    }
    int *list = (int *) calloc(length, sizeof(int));
    if(list == NULL){
        printf("failed to malloc list\n");
        return NULL;
    }
    return list;
}


// Estrutura de um caminho ao longo de um grafo
typedef struct _path_t {
    /* Guarda os índices relativos de visita de um nó; se um valor 
    for igual a 0, significa que ele não foi visitado. */
    int *visits;
    // Comprimento do vetor de visitas
    int length;
    // Contagem de visitas
    int count;
} path_t;


// Inicializa um caminho
int start_path(path_t *path, int size, int start_node) {
    path->visits = generate_list(size);
    path->visits[start_node] = 1;
    path->length = size;
    path->count = 0;
    return EXIT_SUCCESS;
}


// Estrutura de iteração ao longo dos nós adjacentes
typedef struct _adj_iterator_t {
    int node;
    path_t *path;
    int *weights;
    int itr;
} adj_iterator_t;


// Habilita um iterador de adjacências de um nó
// iterator deve ser informado a partir de uma instância pré-existente
int start_adj_iterator(adj_iterator_t *iterator, int node, path_t *path, int *weights) {

    // Atribuição
    iterator->node = node;
    iterator->path = path;
    iterator->weights = weights;
    iterator->itr = 0;

    return EXIT_SUCCESS;
}


// Verifica se há um próximo elemento
int adj_iterator_has_next(adj_iterator_t *iterator){
    return iterator->itr < iterator->path->length;
}


// Retorna o próximo elemento do iterador de adjacências
int adj_iterator_get_next(adj_iterator_t *iterator){
    while (
        iterator->weights[iterator->itr] > 0 && 
        iterator->path->visits[iterator->itr] == 0 && 
        iterator->itr < iterator->path->length
    ){
        iterator->itr = iterator->itr + 1;
    }
    if(iterator->itr >= iterator->path->length){
        return -1;
    }
    return iterator->itr;
}


/**
 * @brief Função recursiva que constrói os caminhos, e retorna, via referência, o ótimo dentre eles.
 * @param graph Grafo representante do problema a ser resolvido.
 * @param optimal_path Caminho ótimo a ser retornado
 * @param current_path Caminho sendo percorrido atualmente.
 */
int recursive_tsp_path(
    graph_t *graph, 
    path_t *optimal_path, 
    path_t *current_path
) {
    return 0;
}


/* Enumera todos os caminhos hamiltonianos, 
imprimindo-os na tela; retorna o menor caminho. */
path_t *enumerate_tsp_paths(graph_t *graph) {

    // Verificação
    if(graph == NULL){
        printf("informed graph is NULL\n");
        return NULL;
    }

    // Informações de caminho
    int *optimal_visits = NULL;
    int optimal_cost = INT_MAX;
    int optimal_count = 0;
    path_t *optimal_path = NULL;
    path_t current_path;

    // Informações de iteração
    int start_node, path_cost;
    adj_iterator_t iterator;

    // Para cada nó de início
    for(start_node=0; start_node<graph->order; ++start_node){

        // Inicialização
        start_path(&current_path, graph->order, start_node);
        start_adj_iterator(&iterator, start_node, &current_path, graph->weights[start_node]);
        path_cost = 0;

        // Finalização
        free(current_path.visits);
    }

    // Finalização
    optimal_path = (path_t *) malloc(sizeof(path_t));
    optimal_path->count = optimal_count;
    optimal_path->length = graph->order;
    optimal_path->visits = optimal_visits;
    return optimal_path;
}


/** 
 * Recupera a lista de adjacências válidas de um nó
 * 
 * Parâmetros:
 * ------------
 * graph:
 *  Grafo em questão.
 * node:
 *  Índice relativo do nó em questão.
 * path: 
 *  Deve ser uma lista com a quantia de nós do grafo fornecido, 
 *  em que mantém a contagem relativa de visita de cada um desses nós. 
 *  Caso um valor seja igual a zero, considerará que o nó correspondente 
 *  não foi visitado.
 * adj:
 *  Lista de adjacências a ser construída.
 * adj_len:
 *  Ponteiro para um inteiro, que receberá o comprimento da lista de adjacências.
 */
int valid_adj (
    graph_t *graph, 
    int node, 
    int *path, 
    int *adj, 
    int *adj_len
) {
    // Desempacotamento (para performance)
    int order = graph->order;
    int **weights = graph->weights;

    // Demais variáveis locais
    int adj_iterator=0, next_node;

    // Verificação dos nós adjacentes àquele informado
    for(next_node=0; next_node<order; ++next_node){
        if(weights[node][next_node] > 0 && path[next_node] <= 0){
            adj[adj_iterator] = next_node;
            ++adj_iterator;
        }
    }
    *adj_len = adj_iterator;

    return EXIT_SUCCESS;
}


int main(int argc, char** argv){
    return 0;
}