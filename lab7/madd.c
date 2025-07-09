#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "matrix.h"

#define     NUM_THREADS     2

typedef struct {
    unsigned int id;
    TMatrix *m, *n, *t;
    unsigned int start_row;
    unsigned int end_row;
} thread_arg_t;

/* the main function of threads */
static void * thread_main(void * p_arg)
{
    thread_arg_t *arg = (thread_arg_t *)p_arg;
    for (unsigned int i = arg->start_row; i < arg->end_row; i++) {
        for (unsigned int j = 0; j < arg->m->ncols; j++) {
            arg->t->data[i][j] = arg->m->data[i][j] + arg->n->data[i][j];
        }
    }
    pthread_exit(NULL);
}

/* Return the sum of two matrices. The result is in a newly created matrix. 
 *
 * If a pthread function fails, report error and exit. 
 * Return NULL if something else is wrong.
 *
 * Similar to addMatrix, but this function uses 2 threads.
 */
TMatrix * addMatrix_thread(TMatrix *m, TMatrix *n)
{
    if (m == NULL || n == NULL || m->nrows != n->nrows || m->ncols != n->ncols)
        return NULL;

    TMatrix *t = newMatrix(m->nrows, m->ncols);
    if (t == NULL)
        return t;

    pthread_t threads[NUM_THREADS];
    thread_arg_t args[NUM_THREADS];

    unsigned int rows_per_thread = m->nrows / NUM_THREADS;

    for (unsigned int i = 0; i < NUM_THREADS; i++) {
        args[i].id = i;
        args[i].m = m;
        args[i].n = n;
        args[i].t = t;
        args[i].start_row = i * rows_per_thread;
        if (i == NUM_THREADS - 1) {
            args[i].end_row = m->nrows; // Ensure last thread covers any remaining rows
        } else {
            args[i].end_row = (i + 1) * rows_per_thread;
        }

        int rc = pthread_create(&threads[i], NULL, thread_main, (void *)&args[i]);
        if (rc) {
            fprintf(stderr, "Error: pthread_create() failed\n");
            freeMatrix(t);
            return NULL;
        }
    }

    for (unsigned int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    return t;
}
