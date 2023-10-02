#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <bits/pthreadtypes.h>

#include "chrono.c"
#include <x86intrin.h>

#define ONE_MILLION 1000000

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
    int initial_input_idx;
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
    
    pair_t *input_pair=(pair_t *)malloc(sizeof(pair_t)*nTotalElements);

    //printf("input_pair:%p\n", input_pair);
    for (int i = 0; i < nTotalElements; i++){
        input_pair[i].val = i;
        //printf("i:%d\n", i);
        //printf("input_pair[i].val:%d\n", input_pair[i].val);
        input_pair[i].key = Input[i];
    }


    //!!elementos iguais podem quebrar todo o processo
    qsort(input_pair, nTotalElements, sizeof(pair_t), comp_keys);


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

    //printf("output:\n");
    //for (int i = 0; i < k; i++)
    //    printf("%f:%d ", Output[i].key, Output[i].val);
    //printf("\n");

    //printf("sorted Input:\n");
    //for (int i = 0; i < compSize; i++)
    //    printf("%f:%d ", input_pair[i].key, input_pair[i].val);
    //printf("\n");

    int ok = 0;
    for (int i = 0; i < compSize; i++){
        for (int j = 0; j < compSize; j++){
            if ( Output[i].val == input_pair[j].val ){
                ok = 1;
                break;
            }
        }
        if (!ok) break;
    }
    
    if( ok )
       printf( "\nOutput set verifyed correctly.\n" );
    else
       printf( "\nOutput set DID NOT compute correctly!!!\n" );   
    free(input_pair);
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
        //printf("i: %d\n", i);
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

void decreaseMax(pair_t heap[], int size, float new_key, int new_value) {
    if (size == 0) // Heap is empty
        return;

    //printf("adding newkey:%f new_value:%d\n", new_key, new_value);
    if( heap[0].key > new_key ) {
      heap[0].key = new_key;
      heap[0].val = new_value;

      //#if SHOW_DECREASE_MAX_STEPS 
      //   drawHeapTree( heap, size, 4 );
      //   printf( "    ~~~~~~~~~~~~~~~~~~~~~~~~~\n" );
      //#endif
      maxHeapify(heap, size, 0);
    }  

}

void *kMenores (void *argument) {
    kmenores_t *arg = (kmenores_t *)argument;

    for (int i = 0; i < arg->k; i++){
        arg->heap[i].val = i+arg->initial_input_idx;
        arg->heap[i].key = arg->Input[i+arg->initial_input_idx];
    }
    //printf("heap:\n");
    //for (int i = 0; i < arg->k; i++)
    //    printf("%f:%d ", arg->heap[i].key,arg->heap[i].val);
    //printf("\n\n");

    maxHeapify(arg->heap, arg->k, 0);
    for (int i = arg->k; i < arg->size; i++){
        //printf("adding decrease %f\n", arg->Input[i+arg->initial_input_idx]);
        //printf("decrease(%p,%d,%f,%d)\n",arg->heap, arg->k, arg->Input[i+arg->initial_input_idx], i+arg->initial_input_idx);
        decreaseMax(arg->heap, arg->k, arg->Input[i+arg->initial_input_idx], i+arg->initial_input_idx);
    }
    pthread_exit(NULL);
    return NULL;
}
pair_t* acharKMenoresSequencial(float* Input, int nTotalElements, int k, int numThreads){
    pthread_t kMenores_threads[numThreads];
    pair_t* result = (pair_t *)malloc(sizeof(pair_t)*k);
    for (int i = 0; i < k; i++){
        result[i].val = i;
        result[i].key = Input[i];
    }
    maxHeapify(result, k, 0);
    for (int i = k; i < nTotalElements; i++)
        decreaseMax(result, k, Input[i], i);

    return result;
}

pair_t* acharKMenores(float* Input, int nTotalElements, int k, int numThreads){

    //printf("acharKMenores input:\n");
    //for (int i = 0; i < nTotalElements; i++)
    //    printf("%f:%d ", Input[i],i);
    //printf("\n");
    //printf("\n");

    int chunk_size = nTotalElements/numThreads;
    int remaining_elements = nTotalElements%numThreads;
    //printf("Calculating chunk size\n");
    while (chunk_size < k)
    {
        numThreads--;
        chunk_size = nTotalElements/numThreads;
        remaining_elements = nTotalElements%numThreads;
    }
    //printf("chunk size calculated\n");
    //printf("numThreads %d\n", numThreads);
    
    pair_t* output = (pair_t *)malloc(sizeof(pair_t)*k*numThreads);
    pthread_t kMenores_threads[numThreads];

    int heap_index;
    int initial_input_idx = 0;
    kmenores_t thread_arguments[numThreads]; 
    for (int n = 0; n < numThreads-1; n++){
        //divide threads
        thread_arguments[n].k = k;
        thread_arguments[n].size = chunk_size;
        thread_arguments[n].Input = Input;
        thread_arguments[n].initial_input_idx = initial_input_idx+n*chunk_size;
        thread_arguments[n].heap = &(output[n*k]);

        //printf("\nthread %d\n", n);
        //printf("k:%d\nsize:%d\ninitial Input idx:%d\nheap pointer:%p\n\n", thread_arguments[n].k, thread_arguments[n].size, thread_arguments[n].initial_input_idx, thread_arguments[n].heap);
        pthread_create( &kMenores_threads[n], NULL, kMenores, (void *)&(thread_arguments[n]) );
    }
    thread_arguments[numThreads-1].k = k;
    thread_arguments[numThreads-1].size = chunk_size+remaining_elements;
    thread_arguments[numThreads-1].Input = Input;
    thread_arguments[numThreads-1].initial_input_idx = initial_input_idx+(numThreads-1)*chunk_size;
    thread_arguments[numThreads-1].heap = &(output[(numThreads-1)*k]);
    //printf("\nthread %d\n", numThreads-1);
    //printf("k:%d\nsize:%d\ninitial Input idx:%d\nheap pointer:%p\n\n", thread_arguments[numThreads-1].k, thread_arguments[numThreads-1].size, thread_arguments[numThreads-1].initial_input_idx, thread_arguments[numThreads-1].heap);
    pthread_create( &kMenores_threads[numThreads-1], NULL, kMenores, (void *)&(thread_arguments[numThreads-1]) );
    for (int i = 0; i < numThreads; i++)
        pthread_join(kMenores_threads[i], NULL);

    //printf("before qsort\n");
    //for (int i = 0; i < k*numThreads; i++)
    //    printf("%f:%d ", output[i].key,output[i].val);
    //printf("\n\n");
    //qsort(output, k*numThreads, sizeof(pair_t), comp_keys);
    //printf("kmenores:\n");
    //for (int i = 0; i < k; i++)
    //    printf("%f:%d ", output[i].key,output[i].val);
    //printf("\n\n");
    ////printf("get kmenores\n");
    //pair_t* kMenores = (pair_t *)malloc(sizeof(pair_t)*k);
    //for (int i = 0; i < k; i++)
    //    kMenores[i] = output[i];

    pair_t* result = (pair_t *)malloc(sizeof(pair_t)*k);
    for (int i = 0; i < k; i++){
        result[i].val = output[i].val;
        result[i].key = output[i].key;
    }
    maxHeapify(result, k, 0);
    for (int i = k; i < k*numThreads; i++)
        decreaseMax(result, k, output[i].key, output[i].val);


    free(output);
    return result;
}

int main(int argc, char **argv){

    chronometer_t parallelReductionTime;

    if (argc != 4){
        fprintf(stderr,"ERRO: número de paramêtros incorreto\n");
        return 1;
    }

    int nTotalElements = atoi(argv[1]);
    int k = atoi(argv[2]);
    int nThreads = atoi(argv[3]);

    // initialize Input vector
    float *Input=(float *)malloc(sizeof(pair_t)*nTotalElements);
    //printf("initializing input array\n");
    int inputSize = 0;
    for( int i = 0; i < nTotalElements; i++ ){

        int a = rand();  // Returns a pseudo-random integer between 0 and RAND_MAX.
        int b = rand();  // same as above
        float v = a * 100.0 + b;
        int p = inputSize;

        // inserir o valor v na posição p
        Input[ p ] = v;
        ++inputSize;
    }
    //printf("Input:\n");
    //for (int i = 0; i < nTotalElements; i++)
    //    printf("%f:%d ", Input[i],i);
    //printf("\n\n");
    pthread_mutex_t *mutex;

    pair_t *Output;
    if (nThreads > 0){
        chrono_reset( &parallelReductionTime );
        chrono_start( &parallelReductionTime );
        Output = acharKMenores(Input, nTotalElements, k, nThreads);
        chrono_stop( &parallelReductionTime );
    }
    else{
        chrono_reset( &parallelReductionTime );
        chrono_start( &parallelReductionTime );
        Output = acharKMenoresSequencial(Input, nTotalElements, k, nThreads);
        chrono_stop( &parallelReductionTime );
    }
    verifyOutput(Input, Output, nTotalElements, k);

    //chrono_reportTime( &parallelReductionTime, "parallelReductionTime" );
    // calcular e imprimir a VAZAO (numero de operacoes/s)
    double total_time_in_seconds = (double) chrono_gettotal( &parallelReductionTime ) /
                                      ((double)1000*1000*1000);
    printf( "Total_time_in_seconds: %lf s\n", total_time_in_seconds );
                                  
    double mop = ((double)nTotalElements * (1 + k))/ONE_MILLION;
    double mops = mop/total_time_in_seconds;
    printf( "Throughput: %lf MOP/s\n", mops );
    free(Input);

    return 0;
}
