/**
 * HEAP.h
 *
 * Max priority queue implementation using binary heaps.
 * Implemented as simple as it gets, with fixed maximal size.
 */

#ifndef HEAP_IMP
#define HEAP_IMP

#include <stdlib.h>

#define HEAP_LEFT(x) (2 * (x) + 1)
#define HEAP_RIGHT(x) (2 * (x) + 2)
#define HEAP_PARENT(x) ((x) / 2)

typedef struct HEAP_s
{
    size_t size;       // number of elements currently in the list
    size_t max_size;   // maximum number of elements
    void ** elems;     // pointer to head of heap
    int * priorities;  // array of element priorities
} HEAP_t;

/**
 * return a new heap with fixed maximal size and element size
 *
 * @param max_size maximal size.
 *
 * return pointer to new heap.
 */
HEAP_t* HEAP_create_heap(size_t max_size);

/**
 * free a heap using given pointer.
 * is NULL safe.
 *
 * @param p_heap pointer to heap to free
 */
void HEAP_free_heap(HEAP_t* p_heap);

/**
 * pop an element from the heap. returning a pointer to the element
 * with maximal priority currently in the heap.
 *
 * @param p_heap pointer to heap to free
 * 
 * @return void* pointer to heap element
 */
void* HEAP_pop(HEAP_t* p_heap);

/**
 * push an element into the heap with given priority.
 * it's size must be at most elem_size.
 *
 * @param p_head pointer to head of list.
 * @param elem element to insert to list.
 * @param priority priority of the element.
 */
void HEAP_push(HEAP_t* p_head, void* elem, int priority);

#endif /*HEAP_IMP*/
