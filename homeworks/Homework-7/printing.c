#define _BSD_SOURCE
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

// Error handling functions for exiting on failure
void die(char *s)
{
    if (errno)
        perror(s); // Print error message from errno
    else 
        fprintf(stderr, "Error: %s\n", s); // Print custom error message
    exit(EXIT_FAILURE); // Exit program on error
}

void check_pthread_return(int rv, char *msg)
{
    if (rv == 0)
        return; // No error, continue
    errno = rv; // Set errno to the error code returned by pthread functions
    die(msg); // Call error handler
}

// Default values for jobs and printers
#define DEFAULT_NUM_JOBS    20
#define DEFAULT_NUM_PRINTERS 2

#define print_job(x) usleep((x)*100 + 50) // Simulate job printing time

// Structure to hold job queue details
typedef struct {
    unsigned char *jobs;            // Array holding job durations
    int njobs_fetched;              // Count of jobs fetched from queue
    int njobs_max;                  // Total jobs available
    pthread_mutex_t mutex;          // Mutex to protect shared job queue
} job_queue_t;

// Structure to represent each printer with its attributes
typedef struct {
    int id;                         // Unique printer ID
    job_queue_t * jq;               // Pointer to shared job queue
    int njobs;                      // Number of jobs completed by this printer
    pthread_t thread_id;            // Thread ID for each printer thread
} printer_t;

/*************  BEGINNING OF QUEUE IMPLEMENTATION *********************/

// Initialize job queue with random job durations
int q_init(job_queue_t *q, int max_jobs)
{
    q->njobs_fetched = 0; // Start with no jobs fetched
    q->njobs_max = max_jobs;
    q->jobs = malloc(max_jobs); // Allocate memory for job array
    if (q->jobs == NULL)
        die("malloc()"); // Exit if memory allocation fails
    for (int i = 0; i < max_jobs; i ++)
        q->jobs[i] = rand() % 100 + 1; // Assign random job durations
    return 0;
}

// Return the number of remaining jobs in the queue
int q_num_jobs(job_queue_t *q)
{
    return (q->njobs_max - q->njobs_fetched);
}

// Fetch a job from the queue for the printer with ID `id`
int q_fetch_job (job_queue_t *q, int id)
{
    int r = q_num_jobs(q);
    if (r <= 0) { // Check if jobs are still available
        fprintf(stderr, "Error: printer %2d tries to get a job "
               "after the max number of jobs have been fetched.", id);
        exit(EXIT_FAILURE);
    }
    printf("Printer  %2d fetched   job %2d\n", id, q->njobs_fetched);
    return q->jobs[q->njobs_fetched++]; // Return job duration and update count
}

// Free memory allocated for the queue's job array
void q_destroy(job_queue_t *q)
{
    if (q->jobs) {
        free(q->jobs); // Free allocated job memory
        q->jobs = NULL;
    }
}

/*************  END OF QUEUE IMPLEMENTATION *********************/

// Print a summary of jobs completed by each printer
void print_printer_summary(printer_t *pprinter, int nprinters)
{
    int total = 0;

    for (int i = 0; i < nprinters; i++, pprinter++) {
        printf("Printer  %2d completed %d jobs.\n", pprinter->id, pprinter->njobs);
        total += pprinter->njobs;
    }
    printf("Total completed is %d.\n", total);
}

// Simulate a single printer fetching and printing jobs
void printer_single(printer_t *pprinter)
{
    int done = 0;
    job_queue_t *jq = pprinter->jq; // Access shared job queue

    pprinter->njobs = 0; // Initialize completed job count
    while (!done) {
        int r = q_num_jobs(jq); // Check remaining jobs in the queue
        if (r <= 0) {           // Exit if no jobs are left
            done = 1;
            continue;
        }
        int job = q_fetch_job(jq, pprinter->id); // Fetch a job from the queue

        print_job(job); // Simulate job processing time

        pprinter->njobs++; // Increment job count for this printer
    }
}

/* Main function for each printer thread */
void *printer_main(void *arg)
{
    int done = 0;
    printer_t *printer = arg;       // Retrieve printer details
    job_queue_t *jq = printer->jq;  // Access shared job queue

    printer->njobs = 0; // Initialize job count for the printer

    while (!done) {
        int d = -1;

        pthread_mutex_lock(&jq->mutex); // Lock mutex to protect shared queue
        d = q_num_jobs(jq);             // Get remaining job count

        if (d <= 0) { // If no jobs are left, mark as done
            done = 1;
            pthread_mutex_unlock(&jq->mutex); // Unlock mutex before exiting
            continue;
        }

        int job = q_fetch_job(jq, printer->id); // Fetch job from queue

        pthread_mutex_unlock(&jq->mutex); // Unlock mutex after fetching job
        
        print_job(job); // Simulate printing the job
        printer->njobs++; // Increment job count for this printer
    }
    return arg;
}

int main(int argc, char *argv[])
{
    int num_printers = DEFAULT_NUM_PRINTERS;
    int num_jobs = DEFAULT_NUM_JOBS;
    int demo = 0;

    // Parse command-line arguments for custom settings
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-p") == 0 && i + 1 < argc) {
            num_printers = atoi(argv[++i]);
            if (num_printers <= 0)
                die("number of printers must be positive.");
        } else if (strcmp(argv[i], "-j") == 0 && i + 1 < argc) {
            num_jobs = atoi(argv[++i]);
            if (num_jobs <= 0)
                die("number of jobs must be positive.");
        } else if (strcmp(argv[i], "-d") == 0) {
            demo = 1;
        } else {
            fprintf(stderr, "Usage: %s [-p N] [-j N]\n", argv[0]);
            fprintf(stderr, "-p <n> : number of printers.\n");
            fprintf(stderr, "-j <n> : total number of jobs.\n");
            fprintf(stderr, "-d     : demo mode.\n");
            return 1;
        }
    }
    printf("num_printers=%d\n", num_printers);
    printf("num_jobs=%d\n", num_jobs);

    int seed = (num_jobs << 24) ^ (num_printers << 8);
    srand(seed); // Seed random generator for job durations

    // Initialize job queue and allocate job durations
    job_queue_t job_queue;
    q_init(&job_queue, num_jobs);

    printer_t printers[num_printers];

    // Demo mode: single printer prints all jobs
    if (demo) {
        printf("Demo: there is only one printer.\n");
        printers[0].id = 0;
        printers[0].jq = &job_queue;
        printer_single(printers);
        q_destroy(&job_queue);
        print_printer_summary(printers, 1);
        exit(0);
    }

    // Initialize mutex for queue protection
    pthread_mutex_init(&job_queue.mutex, NULL);

    // Create threads for each printer
    for (int i = 0; i < num_printers; i++) {
        printers[i].id = i;
        printers[i].jq = &job_queue;
        pthread_create(&printers[i].thread_id, NULL, printer_main, &printers[i]);
    }

    // Wait for all printer threads to complete
    for (int i = 0; i < num_printers; i++) {
        pthread_join(printers[i].thread_id, NULL);
    }

    // Clean up resources
    pthread_mutex_destroy(&job_queue.mutex);
    q_destroy(&job_queue);

    // Print job completion summary
    print_printer_summary(printers, num_printers);

    return 0;
}
