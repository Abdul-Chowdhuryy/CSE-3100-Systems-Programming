#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char **argv) {
    pid_t child1, child2;
    int exitStatus1, exitStatus2;

    if (argc < 4) {
        fprintf(stderr, "Usage: %s cmd1 cmd1_arg cmd2 [cmd2_args ..]\n", argv[0]);
        return 1;
    }

    child1 = fork();
    if (child1 < 0) {
        perror("fork()");
        exit(EXIT_FAILURE);
    }

    if (child1 == 0) {
        execlp(argv[1], argv[1], argv[2], (char *)NULL);
        perror("execlp()");
        exit(EXIT_FAILURE);
    } else {
        waitpid(child1, &exitStatus1, 0);
        printf("exited=%d exitstatus=%d\n", WIFEXITED(exitStatus1), WEXITSTATUS(exitStatus1));

        child2 = fork();
        if (child2 < 0) {
            perror("fork()");
            exit(EXIT_FAILURE);
        }

        if (child2 == 0) {
            execvp(argv[3], &argv[3]);
            perror("execvp()");
            exit(EXIT_FAILURE);
        } else {
            waitpid(child2, &exitStatus2, 0);
            printf("exited=%d exitstatus=%d\n", WIFEXITED(exitStatus2), WEXITSTATUS(exitStatus2));
        }
    }

    return 0;
}

