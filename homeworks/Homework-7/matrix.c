#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "matrix.h"

#define EPSILON 0.0000001  // Precision threshold for comparing matrix elements

// Creates and returns a dynamically allocated matrix with dimensions nrows x ncols
TMatrix * newMatrix(unsigned int nrows, unsigned int ncols)
{
    if (!nrows || !ncols)  // Check for invalid dimensions
        return NULL;

    TMatrix * newM = malloc(sizeof(TMatrix));  // Allocate memory for matrix structure

    TElement ** data = malloc(nrows * sizeof(TElement *));  // Allocate memory for row pointers
    TElement * storage = malloc(nrows * ncols * sizeof(TElement));  // Allocate memory for matrix elements

    // Handle memory allocation failures
    if (newM == NULL || data == NULL || storage == NULL) {
        free(newM);
        free(data);
        free(storage);
        return NULL;
    }

    // Set row pointers to appropriate positions in contiguous memory block
    for (size_t i = 0; i < nrows; i++)
        data[i] = storage + i * ncols;

    // Initialize matrix structure fields
    newM->nrows = nrows;
    newM->ncols = ncols;
    newM->data = data;
    return newM;
}

// Frees all memory associated with the matrix
void freeMatrix(TMatrix * m)
{
    if (m == NULL)  // Check if matrix is already NULL
        return;
    
    if (m->data) {
        free(m->data[0]);  // Free contiguous block of elements
        free(m->data);     // Free array of row pointers
    }
    free(m);  // Free matrix structure
}

// Fills the matrix with random values and returns a pointer to the matrix
TMatrix * fillMatrix(TMatrix * m)
{
    static int first = 1;  // Initialize random seed only once

    if (m == NULL || m->data == NULL)  // Check for invalid matrix
        return m;

    if (first) {  // Seed random number generator once
        srand(3100);
        first = 0;
    }

    for (unsigned int i = 0; i < m->nrows; i++)
        for (unsigned int j = 0; j < m->ncols; j++)
            m->data[i][j] = (TElement)rand();  // Assign random values to elements

    return m;
}

// Returns a new matrix that is the transpose of the input matrix
TMatrix * transposeMatrix(TMatrix * m)
{
    if (m == NULL)  // Check for null input
        return NULL;

    TMatrix * t = newMatrix(m->ncols, m->nrows);  // Allocate transpose matrix
    if (t == NULL)
        return t;

    for (unsigned int i = 0; i < m->nrows; i++)
        for (unsigned int j = 0; j < m->ncols; j++)
            t->data[j][i] = m->data[i][j];  // Swap rows and columns

    return t;
}

// Prints matrix elements, with rows on separate lines
void printMatrix(TMatrix * m)
{
    if (m == NULL)
        return;

    for (unsigned int i = 0; i < m->nrows; i++) {
        for (unsigned int j = 0; j < m->ncols; j++)
            printf("%12g", m->data[i][j]);  // Print each element
        printf("\n");
    }
}

// Compares two matrices, returning 0 if equal, 1 if different, or -1 for invalid input
int compareMatrix(TMatrix * m, TMatrix *n)
{
    if (m == NULL || n == NULL)
        return -1;

    if (m->nrows != n->nrows || m->ncols != n->ncols)
        return -1;

    for (unsigned i = 0; i < m->nrows; i++)
        for (unsigned j = 0; j < m->ncols; j++)
            if (fabs(m->data[i][j] - n->data[i][j]) > EPSILON) {  // Compare elements within EPSILON
                fprintf(stderr, "element[%u][%u] %f %f.\n", i, j, m->data[i][j], n->data[i][j]);
                return 1;
            }

    return 0;
}

// Adds two matrices and returns the result in a newly allocated matrix
TMatrix * addMatrix(TMatrix *m, TMatrix *n)
{
    if (m == NULL || n == NULL || m->nrows != n->nrows || m->ncols != n->ncols)
        return NULL;

    TMatrix * t = newMatrix(m->nrows, m->ncols);  // Allocate matrix for result
    if (t == NULL)
        return t;

    for (unsigned int i = 0; i < m->nrows; i++)
        for (unsigned int j = 0; j < m->ncols; j++)
            t->data[i][j] = m->data[i][j] + n->data[i][j];  // Sum corresponding elements

    return t;
}

// Multiplies two matrices and returns the result in a newly allocated matrix
TMatrix * mulMatrix(TMatrix *m, TMatrix *n)
{
    if (m == NULL || n == NULL || m->ncols != n->nrows)
        return NULL;

    TMatrix * t = newMatrix(m->nrows, n->ncols);  // Allocate matrix for result
    if (t == NULL)
        return t;

    for (unsigned int i = 0; i < m->nrows; i++) {
        for (unsigned int j = 0; j < n->ncols; j++) {
            TElement sum = (TElement)0;
            for (unsigned int k = 0; k < m->ncols; k++)
                sum += m->data[i][k] * n->data[k][j];  // Calculate dot product for each element
            t->data[i][j] = sum;
        }
    }

    return t;
}
