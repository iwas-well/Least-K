#include <stdio.h>
#include <math.h>

// (c) by W.Zola set/23
//
// EXAMPLE Sequential MAX-Heap, 
//                    decreaseMax operation 
//                    and others

// para compilar:
// gcc -O3 max-heap.c -o max-heap -lm

#define SHOW_DECREASE_MAX_STEPS 1

#define MAX_HEAP_SIZE (1024*1024)

void drawHeapTree( int heap[], int size, int nLevels )   // FIX ME!
{
     int offset = 0;
     int space = (int) pow( 2, nLevels-1 );
     //int space = 0;
     
     int nElements = 1;
     for( int level=0; level<nLevels; level++ ) {
        
        // print all elements in this level     
        for( int i=offset; i<size && i<(offset+nElements); i++ ) {
           
           printf( "[%3d]", heap[i] );
           
        }   
        printf( "\n" );
        
        offset += nElements;
        space = nElements-1;
        nElements *= 2;
     }
}



inline void swap( int *a, int *b ) //__attribute__((always_inline));
{
    int temp = *a;
    *a = *b;
    *b = temp;
}

void maxHeapify( int heap[], int size, int i ) 
{
    while (1) {
        int largest = i;
        int left = 2 * i + 1;
        int right = 2 * i + 2;

        if (left < size && heap[left] > heap[largest])
            largest = left;

        if (right < size && heap[right] > heap[largest])
            largest = right;

        if (largest != i) {
            swap(&heap[i], &heap[largest]); // Using the swap function
            i = largest;
            
        } else {
            break;
        }
    }
}

//#define parent(pos) ( pos/2 ) // SE nao usar posicao 0
#define parent(pos) ( (pos-1)/2 )
        
void heapifyUp( int heap[], int *size, int pos ) 
{
     int val = heap[pos];
     while(pos>0 && val > heap[ parent(pos) ]){
         heap[pos] = heap[ parent(pos) ];
         pos = parent(pos);
     }
     heap[pos] = val;
}
        
void insert( int heap[], int *size, int element ) 
{
     *size += 1;
     int last = *size - 1;
     
     heap[last] = element; 
     heapifyUp( heap, size, last );
}


int isMaxHeap( int heap[], int size ) 
{   
    for( int i=1; i<size; i++ )
        if( heap[i] <= heap[parent(i)] )
           continue;
        else {
           printf( "\nbroke at [%d]=%d\n", i, heap[i] );
           printf( "father at [%d]=%d\n", 
                             parent(i), heap[parent(i)] );
           return 0;
        }   
    return 1;       
}



void decreaseMax(int heap[], int size, int new_value) {
    if (size == 0) // Heap is empty
        return;

    if( heap[0] > new_value ) {
      heap[0] = new_value;
      #if SHOW_DECREASE_MAX_STEPS 
         drawHeapTree( heap, size, 4 );
         printf( "    ~~~~~~~~~~~~~~~~~~~~~~~~~\n" );
      #endif
      maxHeapify(heap, size, 0);
    }  
}

//////////////
int main() {
    int heap[ MAX_HEAP_SIZE ] = {60, 50, 40, 30, 20, 10};//{70, 40, 50, 30, 20, 10};
    int heapSize = 6;

    printf(">>>Max-Heap before decrease operation:\n");
    for (int i = 0; i < heapSize; ++i) {
        printf("%d ", heap[i]);
    }
    printf("\n");

    printf("------Max-Heap Tree------\n");
    drawHeapTree( heap, heapSize, 3 );

    decreaseMax(heap, heapSize, 5); // Decreasing the maximum value to 5

    printf("\nMax-Heap after decrease operation:\n");
    for (int i = 0; i < heapSize; ++i) {
        printf("%d ", heap[i]);
    }
    printf("\n");

    printf("------Max-Heap Tree------\n");
    drawHeapTree( heap, heapSize, 3 );

    printf("=========================\n");
    int data[] = {40, 10, 30, 70, 50, 20, 4, 5, 44, 40, 55, 50};
    int n = sizeof(data) / sizeof(data[0]);
    
    
    // empty heap
    heapSize = 0;
    printf("EMPTY heap!\n");
    
    printf("will insert: " );
    for( int i=0; i<n; i++ ) {
      printf("%d ", data[i]);
    }  
    printf( "\n" );
    
    for( int i=0; i<n; i++ ) {
      printf("inserting %d\n", data[i]);
      insert( heap, &heapSize, data[i] );
      printf("------Max-Heap Tree------ ");
      if( isMaxHeap( heap, heapSize ) )
         printf( "is a heap!\n" );
      else
         printf( "is NOT a heap!\n" );
      #ifndef SHOW_DECREASE_MAX_STEPS 
        drawHeapTree( heap, heapSize, 4 );
      #endif   

    }  
    #ifdef SHOW_DECREASE_MAX_STEPS 
       drawHeapTree( heap, heapSize, 4 );
    #endif   
      
    printf("=========================\n");
    printf("=====decreaseMAX tests===\n");
    int data2[] = {4, 10, 30, 70, 55, 20, 4, 5, 25};
    n = sizeof(data2) / sizeof(data2[0]);
    
    
    printf("will decreaseMAX to the following values: " );
    for( int i=0; i<n; i++ ) {
      printf("%d ", data2[i]);
    }  
    printf( "\n" );
    
    for( int i=0; i<n; i++ ) {
      printf("decreaseMAX to %d\n", data2[i]);
      int new_value = data2[i];
      decreaseMax( heap, heapSize, new_value );
      
      printf("------Max-Heap Tree (after decrease)------ ");
      if( isMaxHeap( heap, heapSize ) )
         printf( "is a max heap!\n" );
      else
         printf( "is NOT a max heap!\n" );
         
      //#ifndef SHOW_DECREASE_MAX_STEPS 
       drawHeapTree( heap, heapSize, 4 );
      //#endif   

    }  
     
     
    return 0;
}
