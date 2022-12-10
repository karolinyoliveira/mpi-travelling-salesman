/*
Turma: A

Número do Grupo: 01

Nomes dos integrantes deste grupo que resolveram o trabalho:
Karoliny Oliveira Ozias Silva 10368020
João Lucas Rodrigues Constantino 11795763
Luiz Fernando Silva Eugênio dos Santos 10892680
Alexandre Brito Gomes 11857323
*/

// Dependências
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <limits.h>
#include <mpi.h>
#include <omp.h>

// Grafo orientado com matriz de adjacências
typedef struct _graph_t {
    int order;
    int **weights;
} graph_t;

// Caminho percorrido no grafo
typedef struct _path_t {
    int *nodes;
    int cost;
    int size;
} path_t;

// Booleano
typedef char bool;
#define TRUE 1
#define FALSE 0

// Constantes de ambiente
#define RANDOM_SEED 32
#define START_NODE 0
#define MIN_WEIGHT 1
#define MAX_WEIGHT 100
#define NUM_ATTEMPTS 32

//////////////////////////////// GRAFO ////////////////////////////////////////

// Desaloca um grafo
void free_graph(graph_t *graph){
    int i, order=graph->order;
    for(i=0; i<order; ++i)
        free(graph->weights[i]);
    free(graph->weights);
    free(graph);
}



// Gera um inteiro no intervalo [lb, up]
int generate_int(int lb, int up) {
    return (rand() %(up - lb + 1)) + lb;
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
            weights[i][j] = generate_int(MIN_WEIGHT, MAX_WEIGHT);
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

// Imprime um grafo
void print_graph(graph_t *graph) {
    printf("graph:\n");
    int **w = graph->weights;
    int order=graph->order;
    int i,j;
    for(i=0; i<order; ++i){
        for(j=0; j<order-1; ++j)
            printf("%d ", w[i][j]);
        printf("%d\n", w[i][j]);
    }
    printf("\n");
}

// Cálculo de fatorial
int fact(int n) {
    int fact = n;
    for (int i=1; i<n; i++) {
        fact = fact * i;
    }
    return fact;
}

// Aloca matriz de tamanho adequado para armazenar os caminhos feitos por cada processo
int** build_paths_matrix(int n_cities, int n_paths) {
    int** paths_matrix = (int**) calloc(n_paths, sizeof(int*));
    for (int i=0; i<n_paths; i++) {
        paths_matrix[i] = (int*) calloc(n_cities, sizeof(int));
    }

    return paths_matrix;
}   

///////////////////////// LISTA DE INTEIROS ///////////////////////////////////

// Gera uma lista de inteiros com o tamanho fornecido
int *generate_int_list(int length) {
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


// Copia uma lista de inteiros a outra
void copy_int_list(int *src, int *dest, int len) {
    int i;
    for(i=0; i<len; ++i)
        dest[i] = src[i];
}


// Imprime uma lista de inteiros
void print_int_list(int *list, int len) {
    int i, lim=len-1;
    printf("[");
    for(i=0; i<lim; ++i)
        printf("%d, ", list[i]);
    printf("%d]\n", list[i]);
}


////////////////////////////// CAMINHOS ///////////////////////////////////////

// Inicializa um caminho
void start_path(path_t *path, int size){
    path->nodes = generate_int_list(size);
    path->size = size;
    path->cost = 0;
}


// Obtém o custo de um caminho
void calculate_path_cost(path_t *path, int **weights){
    int i, cost = 0;
    for(i=1; i<path->size; ++i)
        cost += weights[path->nodes[i-1]][path->nodes[i]];
    path->cost = cost;
}


//////////////////// PROBLEMA DO CAIXEIRO VIAJANTE ////////////////////////////

/**
 * @brief Função de recursão da enumeração de caminhos.
 * 
 * @param graph Grafo representante do problema.
 * @param visited Vetor de visitas aos nós. Deve ter comprimento = graph->order.
 * @param opt_path Caminho ótimo local para os nós. Deve ter comprimento = graph->order + 1
 * @param path Caminho dos nós. Deve ter comprimento = graph->order + 1.
 * @param step Passo atual. Não deve nem alcançar nem ultrapassar graph->order.
 * @param node Nó atual. Deve pertencer ao grafo.
 */
int path_enumeration_recursion (
    const graph_t *graph, 
    path_t *opt_path, 
    path_t *path, 
    bool *visited, 
    int step, 
    int node
){
    // Atualização
    path->nodes[step] = node;

    // Variáveis locais
    int cost, next_node, order=graph->order;

    // Finalização do caminho
    if(step == order - 1){

        // Verificação de possibilidade de retorno ao nó inicial
        cost = graph->weights[node][path->nodes[0]];
        if(cost != 0) {

            // Atualização do caminho
            path->nodes[step+1] = path->nodes[0];
            calculate_path_cost(path, graph->weights);

            // Verificação de otimalidade
            if(path->cost < opt_path->cost) {
                copy_int_list(path->nodes, opt_path->nodes, path->size);
                opt_path->cost = path->cost;
            }
            
            return EXIT_SUCCESS;
        }

        return EXIT_FAILURE;
    }

    // Verificação das adjacências do nó
    for(next_node = 0; next_node<order; ++next_node) {

        // Verificação de alcance
        if(visited[next_node] == FALSE) {
            cost = graph->weights[node][next_node] != 0;
            if(cost != 0) {

                // Recursão
                visited[next_node] = TRUE;
                path_enumeration_recursion(graph, opt_path, path, visited, step+1, next_node);

                // Desfaz alterações para manter estabilidade da recursão
                visited[next_node] = FALSE;
            }
        }
    }

    return EXIT_SUCCESS;
}



// Enumeração dos possíveis caminhos do problema do caixeiro viajante considerando primeiro passo dado
path_t start_path_enumeration(const graph_t *graph, int proc_city) {

    // Geração de vetores de utilidade
    int start_node = 0;
    int order = graph->order;
    bool *visited = (bool *) calloc(order, sizeof(bool));
    path_t path, opt_path;

    // Inicialização dos caminhos
    start_path(&path, order+1);
    start_path(&opt_path, order+1);
    opt_path.cost = INT_MAX;
    visited[0] = TRUE;

    // Adicionando primeira execução 
    path.nodes[0] = start_node;

    int cost, next_node=  graph->order;

    next_node = proc_city;
    cost = graph->weights[start_node][next_node] != 0;

    // Recursão
    visited[next_node] = TRUE;
    path_enumeration_recursion(graph, &opt_path, &path, visited, 1, next_node);

    // Finalização
    free(visited);
    free(path.nodes);
    return opt_path;
}


///////////////////////////////// MAIN ////////////////////////////////////////

int main(int argc, char** argv){
    
    // Verificação de argumentos
    if(argc != 2) {
        printf("usage: %s <N>\n", argv[0]);
        return EXIT_FAILURE;
    }

    // Variáveis locais
    int size, rank, tag, provided;
   	MPI_Init_thread(&argc,&argv, MPI_THREAD_MULTIPLE, &provided);
   	MPI_Comm_size(MPI_COMM_WORLD,&size);
   	MPI_Comm_rank(MPI_COMM_WORLD,&rank);
   	MPI_Status status;

    // Obtenção de N e geração do grafo
    int N = atoi(argv[1]);
    graph_t *graph = generate_graph(N, RANDOM_SEED);

    // Programa
    int root_city = 0;
    int Q = (N-1)%(size-1);
    int N_mapped = (N-1-Q)/(size-1); // Número de nós mapeados para cada processo
    N_mapped = N_mapped==0 ? 1 : N_mapped;
    N_mapped = size-1==rank ? N_mapped+Q : N_mapped;
    int n_paths = N_mapped*fact(N)/(N-1);

    //int** best_paths = build_paths_matrix(N+2, size);
    int* best_paths = (int*) calloc(size*(N+2), sizeof(int)); 
    int* path_cost_v = (int*) calloc(N+2, sizeof(int));

    // Processo-mestre
    if (rank == 0) {

        // Alocando para cada processo (com excessão do rank 0)
        int city_index = 1;
        for (int i=1; i<size; i++) {
            for (int j=0; j<N_mapped; j++) {
                MPI_Send(&city_index, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
                city_index++;
            }
        }
        for (int i=1; i<Q+1; i++) {
            MPI_Send(&city_index, 1, MPI_INT, size-1, 0, MPI_COMM_WORLD);
            city_index++;
        }

    } else {

        // Recebendo cidades de cada processo
        int* cities = (int*)calloc(N_mapped, sizeof(int));
        for(int i=0; i<N_mapped; i++) MPI_Recv(&cities[i], 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
        
        int best_cost = INT_MAX;
        int* best_path;
        for(int i=0; i<N_mapped; i++) {
            path_t opt_path = start_path_enumeration(graph, cities[i]);
            if (opt_path.cost < best_cost) { // Checar liberação de memória depois
                best_cost = opt_path.cost;
                best_path = opt_path.nodes;
            }
        }

        // Preparando dados para o gather
        for(int i=0; i<N+1; i++) {
            path_cost_v[i] = best_path[i];
        }
        path_cost_v[N+1] = best_cost;
    }
	
	MPI_Barrier(MPI_COMM_WORLD);
	MPI_Gather(path_cost_v, N+2, MPI_INT, best_paths, N+2, MPI_INT, 0 ,MPI_COMM_WORLD);

    if (rank==0) {

        printf("\n");
        int i, j, argmin=-1, min = INT_MAX;
        for(i=1; i<size; i++) {
            if(best_paths[(N+2)*i + N + 1] < min){
                argmin = i;
                min = best_paths[(N+2)*i + N + 1];
            }
            /* IMPRESSÃO
            for(j=0; j<N+2; j++) {
                printf("%d ", best_paths[(N+2)*i + j ]);
            }
            printf("\n");
            */
        }
        printf("cost = %d; path = [", min);
        for(j=0; j<N; j++) {
            printf("%d, ", best_paths[(N+2)*argmin + j ]);
        }
        printf("%d]\n", best_paths[(N+2)*argmin + j ]);
    }

    // Finalização
    free_graph(graph);
    free(best_paths);
    free(path_cost_v);
    MPI_Finalize();
    return EXIT_SUCCESS;
}