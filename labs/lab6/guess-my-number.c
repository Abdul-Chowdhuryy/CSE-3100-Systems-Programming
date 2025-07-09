#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>
#include <sys/wait.h>
#include <errno.h>
#include <sys/types.h>
#include <dirent.h>

void die(char *s) {
    fprintf(stderr, "Error: %s\n", s);
    if (errno)
        perror("errno");
    exit(EXIT_FAILURE);
}

#define MAX_VALUE       1000
#define MSG_BUF_SIZE    100
#define PIPEFD_READ 0
#define PIPEFD_WRITE 1

typedef struct {
    int v;
    int num_attempts;
    char message[MSG_BUF_SIZE];
} gmn_t;

void gmn_init(gmn_t *pg, int seed) {
    srand(seed);
    pg->v = rand() % MAX_VALUE + 1;
    pg->num_attempts = 0;
    strcpy(pg->message, "You have not got the right number yet.\n");
}

int gmn_check(gmn_t *pg, int guess) {
    pg->num_attempts++;
    if (pg->v == guess) {
        snprintf(pg->message, MSG_BUF_SIZE, 
                 "It took you %d attempt(s) to guess the number %d.\n", 
                 pg->num_attempts, pg->v);
        return 0;
    }
    if (pg->v > guess)
        return 1;
    return -1;
}

char *gmn_get_message(gmn_t *pg) {
    return pg->message;
}

int gmn_get_max() {
    return MAX_VALUE;
}

void guess_my_number(int seed) {
    gmn_t gmn;
    gmn_init(&gmn, seed);
    int min = 1;
    int max = gmn_get_max();
    int result;
    do {
        int guess = (min + max) / 2;
        printf("My guess: %d\n", guess);
        result = gmn_check(&gmn, guess);
        if (result > 0)
            min = guess + 1;
        else if (result < 0)
            max = guess - 1;
    } while (result != 0);
    fputs(gmn_get_message(&gmn), stdout);
}

void child_main(int fdp[], int fdc[], int seed) {
    gmn_t gmn;
    gmn_init(&gmn, seed);
    
    close(fdp[PIPEFD_WRITE]);
    close(fdc[PIPEFD_READ]);
    
    int max_value = gmn_get_max();
    write(fdc[PIPEFD_WRITE], &max_value, sizeof(max_value));
    
    int guess;
    int result;
    while (1) {
        read(fdp[PIPEFD_READ], &guess, sizeof(guess));
        result = gmn_check(&gmn, guess);
        write(fdc[PIPEFD_WRITE], &result, sizeof(result));
        if (result == 0) {
            write(fdc[PIPEFD_WRITE], gmn_get_message(&gmn), MSG_BUF_SIZE);
            break;
        }
    }
    
    close(fdp[PIPEFD_READ]);
    close(fdc[PIPEFD_WRITE]);
    
    exit(EXIT_SUCCESS);
}

void print_file_descriptors(int child);

int main(int argc, char *argv[]) {
    int seed = 3100;
    int demo = 0;

    for (int i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "demo")) {
            demo = 1;
        } else if (!strcmp(argv[i], "-h")) {
            fprintf(stderr, "Usage: %s [<seed>] [demo]\n", argv[0]);
            return 1;
        } else {
            seed = atoi(argv[i]);
            if (seed <= 0)
                die("seed must be a positive number.");
        }
    }

    if (demo) {
        guess_my_number(seed);
        exit(0);
    }

    int fdp[2], fdc[2];
    if (pipe(fdp) == -1)
        die("pipe() failed.");
    if (pipe(fdc) == -1)
        die("pipe() failed.");

    pid_t pid;
    pid = fork();
    if (pid < 0)
        die("fork() failed.");
    if (pid == 0)
        child_main(fdp, fdc, seed);

    close(fdp[PIPEFD_READ]);
    close(fdc[PIPEFD_WRITE]);

    int min = 1;
    int max;
    read(fdc[PIPEFD_READ], &max, sizeof(max));
    
    int guess;
    int result;

    do {
        guess = (min + max) / 2;
        printf("My guess: %d\n", guess);
        write(fdp[PIPEFD_WRITE], &guess, sizeof(guess));
        read(fdc[PIPEFD_READ], &result, sizeof(result));
        if (result > 0)
            min = guess + 1;
        else if (result < 0)
            max = guess - 1;
    } while (result != 0);

    char message[MSG_BUF_SIZE];
    read(fdc[PIPEFD_READ], message, MSG_BUF_SIZE);
    fputs(message, stdout);

    close(fdp[PIPEFD_WRITE]);
    close(fdc[PIPEFD_READ]);

    wait(NULL);

    return 0;
}
