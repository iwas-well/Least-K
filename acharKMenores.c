#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <bits/pthreadtypes.h>

//  tentar divisao e conquista
//  multiplos processos pegam menores de parte do vetor original
//  apos a finalizção de um deles, utilizar a barreira

//  usage: ./acharKMenores <nTotalElements> <k> <nThreads>

// ENTRADA para o algoritmo:
// A entrada para a função acharKMenores será: 
//   um vetor (GLOBAL) de floats com nTotalElements, 
//   cada elemento deve ser um valor float
// Esse conjunto de entrada, deve ser chamado Input

#define MAX_SIZE 100000000

typedef struct {
    float key;   // inserir um valor v float na chave 
                 //  (obtido do vetor de entrada Input)
    int val;     // inserir a posiçao p como valor val
} pair_t;


typedef struct {
    float *Input;
    int initial_index;
    int size;
    pair_t *heap;
    int k; 
} kmenores_t;

static int comp_keys(const void* par1, const void* par2){
    if ( ((pair_t *)par1)->key < ((pair_t *)par2)->key ) return -1;
    return 1;
}

void verifyOutput( const float *Input, 
                   const pair_t *Output,   // pair_t é o tipo de um par (v,p)
                      int nTotalElements,
                      int k )
{
    // codigo da verificacao a ser incluido por voce
    // voce deve verificar se o conjunto de pares de saida está correto
    // e imprimir uma das mensagens abaixo
    
    pair_t input_pair[nTotalElements];

    for (int i = 0; i < nTotalElements; i++){
        input_pair[0].val = i;
        input_pair[0].key = Input[i];
    }

    //!!elementos iguais podem quebrar todo o processo
    qsort(input_pair, nTotalElements, sizeof(pair_t), comp_keys);

    printf("solucao:\n");
    for (int i = 0; i < k; i++)
        printf("%f ", input_pair[i].key);
    printf("\n");

    int x = input_pair[0].val;
    int num_dif = 1;
    int compSize = 1;
    for (int i = 1; i < nTotalElements; i++){
        if (x != input_pair[i].val){
            num_dif++;
            if (num_dif > k) break;
            x = input_pair[i].val;
        }
        compSize++;
    }

    int ok = 1;
    for (int i = 0; i < compSize; i++){
        for (int j = 0; j < compSize; j++){
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


void swap(pair_t *a, pair_t *b) //__attribute__((always_inline));
{
    pair_t temp = *a;
    *a = *b;
    *b = temp;
}

void maxHeapify(pair_t heap[], int size, int i) 
{
    while (1) {
        printf("i: %d\n", i);
        int largest = i;
        int left = 2 * i + 1;
        int right = 2 * i + 2;

        if (left < size && heap[left].key > heap[largest].key)
            largest = left;

        if (right < size && heap[right].key > heap[largest].key)
            largest = right;

        if (largest != i) {
            swap( &(heap[i]) , &(heap[largest]) ); // Using the swap function
            i = largest;
            
        } else {
            break;
        }
    }
}

void decreaseMax(pair_t heap[], int size, int new_key, int new_value) {
    if (size == 0) // Heap is empty
        return;

    if( heap[0].key > new_value ) {
      heap[0].key = new_key;
      heap[0].val = new_value;

      #if SHOW_DECREASE_MAX_STEPS 
         drawHeapTree( heap, size, 4 );
         printf( "    ~~~~~~~~~~~~~~~~~~~~~~~~~\n" );
      #endif
      maxHeapify(heap, size, 0);
    }  

}

void *kMenores (void *argument) {
//void kMenores (float Input[], int initial_index, int size, pair_t output[], int k, int threadId) {
    kmenores_t *arg = (kmenores_t *)argument;
    for (int i = arg->initial_index; i < arg->k; i++){
        int heap_index = i - arg->initial_index;
        arg->heap[heap_index].val = i;
        arg->heap[heap_index].key = arg->Input[i];
    }

    maxHeapify(arg->heap, arg->k, 0);
    for (int i = arg->k; i < arg->size; i++)
        decreaseMax(arg->heap, arg->k, arg->Input[i], i);

    pthread_exit(NULL);
    return NULL;
}

pair_t* acharKMenores(float* Input, int nTotalElements, int k, int numThreads){

    int chunk_size = 0;
    int remaining_elements = 0;

    while (chunk_size < k)
    {
        chunk_size = nTotalElements/numThreads;
        remaining_elements = nTotalElements%numThreads;
        numThreads--;
    }

    printf("numThreads %d\n", numThreads);
    pair_t* output = malloc(sizeof(pair_t)*k*numThreads);
    pthread_t kMenores_threads[numThreads];

    int heap_index;
    int initial_index = 0;
    kmenores_t thread_arguments[numThreads]; 
    for (int n = 0; n < numThreads-1; n++){
        //divide threads
        thread_arguments[n].k = k;
        thread_arguments[n].size = chunk_size;
        thread_arguments[n].Input = Input;
        thread_arguments[n].initial_index = initial_index;
        printf("output%d %p\n",n, &(output[n*k]));
        thread_arguments[n].heap = &(output[n*k]);

        pthread_create( &kMenores_threads[n], NULL, kMenores, (void *)&(thread_arguments[n]) );
        //kMenores(Input, initial_index, chunk_size, &(output[n*k]), k, threadId[n]);
        initial_index += chunk_size;
    }
    thread_arguments[numThreads-1].k = k;
    thread_arguments[numThreads-1].size = chunk_size+remaining_elements;
    thread_arguments[numThreads-1].Input = Input;
    thread_arguments[numThreads-1].initial_index = initial_index;
    thread_arguments[numThreads-1].heap = &(output[(numThreads-1)*k]);
    printf("output%d %p\n",numThreads-1, &(output[(numThreads-1)*k]));
    pthread_create( &kMenores_threads[numThreads-1], NULL, kMenores, (void *)&(thread_arguments[numThreads-1]) );
    //kMenores(Input, initial_index, chunk_size+remaining_elements, \
    //        &(output[(numThreads-1)*k]), k, threadId[numThreads-1]);

    for (int i = 0; i < numThreads; i++)
        pthread_join(kMenores_threads[i], NULL);

    printf("qsort\n");
    qsort(output, k*numThreads, sizeof(pair_t), comp_keys);

    printf("get kmenores\n");
    pair_t* kMenores = malloc(sizeof(pair_t)*k);
    for (int i = 0; i < k; i++)
        kMenores[i] = output[i];

    free(output);
   
    return kMenores;
}

int main(int argc, char **argv){

    int nTotalElements = atoi(argv[1]);
    int k = atoi(argv[2]);
    int nThreads = atoi(argv[3]);

    printf("initializa\n");
    // initialize Input vector
    float Input[nTotalElements];
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

    //o programa deve calcular e imprimir 
    //  o tempo e a vazão do algoritmo acharKMenores usando
    //  a inicializacao de um Max-Heap de tamanho K
    //  a VERSÂO 1 do algoritmo usando a operação decreaseMAX
    //  e as idéias descritas
    //  no arquivo : 
    //    max-heap-descricao-e-examplos.pdf
    //
    // A vazão deve ser reportada em MOPs (Mega Operacoes por segundo)
    // que é o numero de operações de insersão+decreaseMax
    // feitas por segundo no Max-Heap pelo seu algoritmo paralelo para
    // uma dada quantidade de threads.

    printf("achar menores\n");
    pair_t *Output = acharKMenores(Input, nTotalElements, k, nThreads);

    for (int i = 0; i < k; i++)
        printf("%f ", Output[i].key);
    printf("\n");

    verifyOutput(Input, Output, nTotalElements, k);

    // SAIDA do o algoritmo:
    // A saída da função acharKMenores será: 
    //   um conjunto de k elementos do tipo (chave, valor)
    //   conforme especificado acima,
    //   esse conjunto de saída deve ser chamado de Output
    return 0;
}
