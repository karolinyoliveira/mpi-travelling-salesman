/*
Turma: A

Número do Grupo: 01

Nomes dos integrantes deste grupo que resolveram o trabalho:
Karoliny Oliveira Ozias Silva 10368020
João Lucas Rodrigues Constantino 11795763
Luiz Fernando Silva Eugênio dos Santos 10892680
Alexandre Brito Gomes 11857323
*/

// Dedendências
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
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

    int N = atoi(argv[1]);
    graph_t *graph = generate_graph(N, RANDOM_SEED);

    // Programa
    //print_graph(graph);
    int Q = (N-1)%(size-1);
    int N_mapped = (N-1-Q)/(size-1); // Número de nós mapeados para cada processo
    N_mapped = N_mapped==0 ? 1 : N_mapped;


    // Finalização
    free_graph(graph);
    MPI_Finalize();
    return EXIT_SUCCESS;

    /*
    int process_rank, size_Of_Cluster;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size_Of_Cluster);
    MPI_Comm_rank(MPI_COMM_WORLD, &process_rank);

    printf("Hello World from process %d of %d\n", process_rank, size_Of_Cluster);

    MPI_Finalize();
    return 0;
    */
}