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
        paths_matrix[i] = (int*) calloc(n_cities+1, sizeof(int));
    }

    return paths_matrix;
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
    int root_city = 0;
    int Q = (N-1)%(size-1);
    int N_mapped = (N-1-Q)/(size-1); // Número de nós mapeados para cada processo
    N_mapped = N_mapped==0 ? 1 : N_mapped;
    N_mapped = size-1==rank ? N_mapped+Q : N_mapped;
    int n_paths = N_mapped*fact(N)/(N-1);

    int** all_paths = build_paths_matrix(N, n_paths);
    int** best_paths = build_paths_matrix(N, size);

    if (rank == 0) {

        // Alocando para cada processo (com excessão do rank 0)
        int city_index = 1;
        for (int i=0; i<size-1; i++) {
            for (int j=0; j<N_mapped; j++) {
                MPI_Send(&city_index, 1, MPI_INT, i+1, 0, MPI_COMM_WORLD);
                city_index++;
            }
        }
        for (int i=0; i<Q; i++) {
            if (city_index != 0) {
                MPI_Send(&city_index, 1, MPI_INT, i+1, 0, MPI_COMM_WORLD);
            } 
            city_index++;
        }
    } else {
        int* vetores_iniciais = (int*)calloc(N_mapped, sizeof(int));
		
        for(int i=0; i<N_mapped; i++) MPI_Recv(&vetores_iniciais[i], 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
        for(int i=0; i<N_mapped; i++) {
            printf("%d\n", vetores_iniciais[i]);
        }
        
        
        //int teste;
        //MPI_Recv(&teste, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
        //printf("%d\n", teste);
        
    }


    // Finalização
    free_graph(graph);
    MPI_Finalize();
    return EXIT_SUCCESS;
}