#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

//  usage: ./acharKMenores <nTotalElements> <k> <nThreads>

// ENTRADA para o algoritmo:
// A entrada para a função acharKMenores será: 
//   um vetor (GLOBAL) de floats com nTotalElements, 
//   cada elemento deve ser um valor float
// Esse conjunto de entrada, deve ser chamado Input

#define MAX_SIZE 10

typedef struct {
    float key;   // inserir um valor v float na chave 
                 //  (obtido do vetor de entrada Input)
    int val;     // inserir a posiçao p como valor val
} pair_t;

static int comp_keys(const void* par1, const void* par2){

    if ( ((pair_t *)par1)->key < ((pair_t *)par2)->key ) return -1;
    return 1;
}

void verifyOutput( const float *Input, 
                   const pair_t *Output,   // pair_t é o tipo de um par (v,p)
                      int nTotalElmts,
                      int k )
{
    // codigo da verificacao a ser incluido por voce
    // voce deve verificar se o conjunto de pares de saida está correto
    // e imprimir uma das mensagens abaixo
    int ok = 1;
    
    pair_t input_pair[nTotalElmts];

    for (int i = 0; i < nTotalElmts; i++){
        input_pair[0].val = i;
        input_pair[0].key = Input[i];
    }

    ok = 1;
    //!!elementos iguais podem quebrar todo o processo
    qsort(input_pair, nTotalElmts, sizeof(pair_t), comp_keys);
    for (int i = 0; i < k; i++){
        for (int j = 0; j < k; j++){
            if ( Output[i].val != input_pair[j].val ){
                ok = 0;
                break;
            }
        }
        if (!ok) break;
    }
    
    if( ok )
       printf( "\nOutput set verifyed correctly.\n" );
    else
       printf( "\nOutput set DID NOT compute correctly!!!\n" );   
}

int main(int argc, char **argv){

    int nTotalElements = atoi(argv[1]);
    int k = atoi(argv[2]);
    int nThreads = atoi(argv[3]);

    // initialize Input vector
    float Input[MAX_SIZE];
    int inputSize = 0;
    for( int i = 0; i < nTotalElements; i++ ){

        int a = rand();  // Returns a pseudo-random integer
                         //    between 0 and RAND_MAX.
        int b = rand();  // same as above

        float v = a * 100.0 + b;
        int p = inputSize;

        // inserir o valor v na posição p
        Input[ p ] = v;
        ++inputSize;
    }
    pthread_mutex_t *mutex;
    pthread_mutex_lock(mutex);
    pthread_mutex_unlock(mutex);

    // o programa deve calcular e imprimir 
    //   o tempo e a vazão do algoritmo acharKMenores usando
    //   a inicializacao de um Max-Heap de tamanho K
    //   a VERSÂO 1 do algoritmo usando a operação decreaseMAX
    //   e as idéias descritas
    //   no arquivo : 
    //     max-heap-descricao-e-examplos.pdf
    //
    //  A vazão deve ser reportada em MOPs (Mega Operacoes por segundo)
    //  que é o numero de operações de insersão+decreaseMax
    //  feitas por segundo no Max-Heap pelo seu algoritmo paralelo para
    //  uma dada quantidade de threads.

    pair_t Output[k];
    verifyOutput(Input, Output, nTotalElements, k);

    // SAIDA do o algoritmo:
    // A saída da função acharKMenores será: 
    //   um conjunto de k elementos do tipo (chave, valor)
    //   conforme especificado acima,
    //   esse conjunto de saída deve ser chamado de Output
    return 0;
}
