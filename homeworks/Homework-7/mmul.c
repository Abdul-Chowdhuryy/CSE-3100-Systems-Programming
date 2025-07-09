#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "matrix.h"

// Number of threads to be used for matrix multiplication
#define NUM_THREADS 2

// Struct to pass arguments to threads
typedef struct {
    unsigned int id;       // Thread ID (0 for first half, 1 for second half)
    TMatrix *m, *n, *t;    // Pointers to matrices: m (left), n (right), t (result)
} thread_arg_t;

// Function executed by each thread to perform partial matrix multiplication
static void * thread_main(void * p_arg)
{
    // Cast the argument to thread_arg_t type
    thread_arg_t * t_arg = (thread_arg_t *)p_arg;

    // Variables to define the range of rows this thread will handle
    int start, end;

    // Divide work based on thread ID:
    // Thread 0 processes the first half of rows; thread 1 processes the second half
    if(t_arg->id == 0) {
        start = 0;                           // Start from the first row
        end = t_arg->m->nrows / 2;           // End at the middle row
    } else {
        start = t_arg->m->nrows / 2;         // Start from the middle row
        end = t_arg->m->nrows;               // End at the last row
    }

    // Nested loop to go through assigned rows and perform multiplication
    for(int i = start; i < end; i++) {
        for(int j = 0; j < t_arg->m->ncols; j++) {
            TElement sum = (TElement)0;      // Initialize sum for each cell in result matrix
            for(int k = 0; k < t_arg->m->ncols; k++) {
                // Multiply corresponding elements and accumulate in sum
                sum += t_arg->m->data[i][k] * t_arg->n->data[k][j];
            }
            // Store computed sum in the result matrix t at position (i, j)
            t_arg->t->data[i][j] = sum;
        }
    }

    return NULL; // Thread finishes
}

/* Multiply two matrices using multiple threads.
 *
 * Returns a new matrix t, the product of matrices m and n.
 * Returns NULL if any input is invalid or memory allocation fails.
 */
TMatrix * mulMatrix_thread(TMatrix *m, TMatrix *n)
{
    // Check if input matrices are compatible for multiplication
    if (m == NULL || n == NULL || m->ncols != n->nrows)
        return NULL; // Return NULL if matrices cannot be multiplied

    // Create a new matrix to store the result of the multiplication
    TMatrix * t = newMatrix(m->nrows, n->ncols);
    if (t == NULL)
        return t; // Return NULL if memory allocation fails

    // Initialize argument structs for each thread
    thread_arg_t arg1 = {0, m, n, t}; // For first half of rows
    thread_arg_t arg2 = {1, m, n, t}; // For second half of rows

    // Array to hold thread identifiers
    pthread_t threads[NUM_THREADS];

    // Create threads for matrix multiplication
    // If thread creation fails, report error and exit
    if(pthread_create(&threads[0], NULL, thread_main, &arg1) != 0) {
        perror("pthread_create");
        exit(1);
    } 
    if(pthread_create(&threads[1], NULL, thread_main, &arg2) != 0) {
        perror("pthread_create");
        exit(1);
    }

    // Wait for both threads to complete execution
    // If join fails, report error and exit
    if(pthread_join(threads[0], NULL) != 0) {
        perror("pthread_join");
        exit(1);
    } 
    if(pthread_join(threads[1], NULL) != 0) {
        perror("pthread_join");
        exit(1);
    }

    // Return the result matrix after both threads have completed
    return t;
}
