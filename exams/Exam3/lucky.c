
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include <assert.h>
#define MAX 100000
#define SIZE 10000

int n = SIZE;           
int v[SIZE][SIZE];      //2d array
pthread_mutex_t mutex;  //a mutex
int r[MAX];             //1d array to save results
int count = 0;          //number of items in the array r

void die(char *s)
{
    fprintf(stderr, "Error: %s\n", s);
    if (errno) perror("errno");
    exit(EXIT_FAILURE);
}

void check_pthread_return(int rv, char *msg)
{
    if (rv == 0) return;
    errno = rv;
    die(msg);
}

//TODO:
//implement the following function to append i*n + j to
//the end of array r
//and update count
//note multiple thread could call this function simutaneously
void append(int i, int j)
{
    pthread_mutex_lock(&mutex);
    r[count++] = i * n + j;         // appends i*n + j 
    pthread_mutex_unlock(&mutex);
}

typedef struct thread_arg_tag {
    int row1, row2;
} thread_arg_t;

//TODO:
//implement the following function
//to test whether a 3x3 square centered at (i, j) is a lucky square

int lucky_square(int i, int j)
{
    int total = 0;
    for (int n = 0; n < 9; n++) {
        total += v[i - 1 + n / 3][j - 1 + n % 3]; // Tests whether a 3x3 is square centered at i, j is a lucky
    }
    return total % 10 == 0;
}

// print_square function is provided to help debugging 
void print_square(int i, int j)
{
    printf("v[%d][%d] = %d\t", i, j, v[i][j]);
    for(int k = 0; k < 8; k++)
    {
        int val = v[i -1 + k /3][j - 1 + k %3];
        printf("%d ", val);
    }
    int k = 9;
    int val = v[i -1 + k /3][j - 1 + k %3];
    printf("%d\n", val);
}

// main for threads
void* thread_main(void* thread_arg) {
    thread_arg_t* arg = thread_arg;
    
    int row1 = arg->row1;
    int row2 = arg->row2;

    for(int i = row1; i <= row2; i++)
    for(int j = 1; j < n - 1; j++)
    {
        if(lucky_square(i, j)) append(i, j);
    }
    pthread_exit(NULL);
}

// int_cmp used for qsort()
int int_cmp(const void *a, const void *b)
{
    return (*(int *)a - *(int *)b);
}

int main(int argc, char* argv[]) {

    if(argc != 3)
    {
        printf("Usage: %s seed n_threads\n", argv[0]);
        return 0;
    }

    int seed = atoi(argv[1]);
    int n_threads = atoi(argv[2]);
    srand(seed);

    // initialization
    for(int i = 0; i < n; i++)
    for(int j = 0; j < n; j++)
    {
        v[i][j] = rand() % 10;
    }

  //TODO: initialize something 
    pthread_mutex_init(&mutex, NULL); // Initialized a thread

    pthread_t thread_arr[n_threads];
    thread_arg_t thread_args[n_threads];
    int rv;

    //TODO: Set up thread arguments
    int rows_thread = n / n_threads;
    for (int i = 0; i < n_threads; i++) {
        thread_args[i].row1 = i * rows_thread;
        thread_args[i].row2 = (i == n_threads - 1) ? n - 1 : (i + 1) * rows_thread - 1; // Setting up thread
    }

    //TODO: Create the threads
    for (int i = 0; i < n_threads; i++) {
        rv = pthread_create(&thread_arr[i], NULL, thread_main, &thread_args[i]); // Creating threaad
        check_pthread_return(rv, "pthread_create");
    }

    //TODO: Join with the threads
    for (int i = 0; i < n_threads; i++) {
        rv = pthread_join(thread_arr[i], NULL); // Joining threads
        check_pthread_return(rv, "pthread_join");
    }

    //TODO: Destroy the mutex
    pthread_mutex_destroy(&mutex);

    // DO NOT CHANGE THE FOLLOWING CODE
    printf("Total lucky squares: %d\n", count);
    // sort r[] and print results
    qsort(r, count, sizeof(int), int_cmp);
    for(int i = 0; i < (10 < count ? 10 : count); i++)
    {
        printf("(%d, %d)\n", r[i] / n, r[i] % n);
    }

    return 0;
}