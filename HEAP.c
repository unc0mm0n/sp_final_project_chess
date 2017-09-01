#include <string.h>
#include <assert.h>
#include <stdio.h>

#include "HEAP.h"

/** swap positions of two elements of the heap */
void _HEAP_swap(HEAP_t* p_heap, size_t f_idx, size_t s_idx)
{
        // save
        void* tmp = p_heap->elems[s_idx];
        int tmp_p = p_heap->priorities[s_idx];
        
        // overwrite
        p_heap->priorities[s_idx] = p_heap->priorities[f_idx];
        p_heap->elems[s_idx] = p_heap->elems[f_idx];

        // restore
        p_heap->priorities[f_idx] = tmp_p;
        p_heap->elems[f_idx] = tmp;
}

/**
 * bubble elements down the heap
 */
void _HEAP_bubble_down(HEAP_t* p_heap, size_t idx)
{
    size_t l_idx = HEAP_LEFT(idx);
    size_t r_idx = HEAP_RIGHT(idx);

    if (l_idx >= p_heap->size) // no children
    {
        return;
    }
    
    size_t max_idx = l_idx;
    if (r_idx < p_heap->size && p_heap->priorities[l_idx] < p_heap->priorities[r_idx]) // if right element is larger
    {
        max_idx = r_idx;
    }

    if (p_heap->priorities[max_idx] > p_heap->priorities[idx])
    {
        _HEAP_swap(p_heap, idx, max_idx);
        _HEAP_bubble_down(p_heap, max_idx);
    }
}

/**
 * bubble elements up the heap.
 */
void _HEAP_bubble_up(HEAP_t* p_heap, size_t idx)
{
    assert (idx < p_heap->size);
    if (idx == 0)
    {
        return;
    }

    size_t parent_idx = HEAP_PARENT(idx);
    if (p_heap->priorities[parent_idx] < p_heap->priorities[idx])
    {
        _HEAP_swap(p_heap, idx, parent_idx);
        _HEAP_bubble_up(p_heap, parent_idx);
    }
}

HEAP_t* HEAP_create_heap(size_t max_size)
{
    HEAP_t* p_heap = malloc(sizeof(HEAP_t));
    if (p_heap == NULL)
    {
        return NULL;
    }

    p_heap->priorities = malloc(max_size * sizeof(int));
    if (p_heap->priorities == NULL)
    {
        free(p_heap);
        return NULL;
    }

    p_heap->elems = malloc(max_size * sizeof(void*));
    if (p_heap->elems == NULL)
    {
        free(p_heap->priorities);
        free(p_heap);
        return NULL;
    }

    p_heap->max_size = max_size;
    p_heap->size = 0;

    return p_heap;
}

void HEAP_free_heap(HEAP_t* p_heap)
{
    free(p_heap->elems);
    free(p_heap->priorities);
    free(p_heap);
}

void* HEAP_pop(HEAP_t* p_heap)
{
    assert (p_heap->size > 0);
    void* ret = p_heap->elems[0]; // get element

    // move last to first
    if (p_heap->size > 1)
    {
        p_heap->elems[0] = p_heap->elems[p_heap->size - 1];
        p_heap->priorities[0] = p_heap->priorities[p_heap->size - 1];
        _HEAP_bubble_down(p_heap, 0);
    }
    p_heap->size--;
    return ret;
}

void HEAP_push(HEAP_t* p_heap, void* elem, int priority)
{
    assert(p_heap->size < p_heap->max_size);
    p_heap->size++;
    p_heap->elems[p_heap->size-1] = elem;
    p_heap->priorities[p_heap->size-1] = priority;
    _HEAP_bubble_up(p_heap, p_heap->size-1);
}


/* Tests * / 
void _HEAP_test_print(const HEAP_t* p_heap)
{
    printf("Heap priorities:\n");
   for (size_t i=0; i < p_heap->size; i++)
   {
        printf("%d, ", p_heap->priorities[i]);
        int tmp = i+2;
        while(tmp >= 1 && (tmp & 1) == 0)
        {
            tmp = tmp >> 1;
        }
        if (tmp == 0 || tmp == 1)
        {
            printf("\n");
        }
   }
   printf("\n");
   printf("Heap size: %lu\n", p_heap->size);
}

int main(){

    int i = 7;

    HEAP_t* p_heap = HEAP_create_heap(1000, sizeof(int));
    HEAP_push(p_heap, &i, 8);
    _HEAP_test_print(p_heap);

    HEAP_push(p_heap, (void *) p_heap, 9);
    HEAP_push(p_heap, (void *) p_heap, 9);

    HEAP_push(p_heap, (void *) p_heap, 9);
    HEAP_push(p_heap, (void *) p_heap, 9);
    HEAP_push(p_heap, (void *) p_heap, 9);
    HEAP_push(p_heap, (void *) p_heap, 9);
    HEAP_push(p_heap, (void *) p_heap, 9);
    HEAP_push(p_heap, (void *) p_heap, 9);
    HEAP_push(p_heap, (void *) p_heap, 9);
    HEAP_push(p_heap, (void *) p_heap, 9);
    HEAP_push(p_heap, (void *) p_heap, 4);
    HEAP_push(p_heap, (void *) &i, 11);
    HEAP_push(p_heap, (void *) p_heap, 4);
    _HEAP_test_print(p_heap);

    int * p_i = (int *) HEAP_pop(p_heap);
    printf("=%d=\n", *(p_i));
    _HEAP_test_print(p_heap);
    
    free(p_i);
    HEAP_free_heap(p_heap);
    return 0;
}// */
