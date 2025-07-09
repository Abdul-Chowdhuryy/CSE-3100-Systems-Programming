#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <errno.h>

#define MAX_NUM_PROGRAMS 10
#define FD_STDIN 0
#define FD_STDOUT 1
#define PIPEFD_READ 0
#define PIPEFD_WRITE 1

void die(char *s) {
    fprintf(stderr, "Error: %s\n", s);
    if (errno)
        perror("errno");
    exit(EXIT_FAILURE);
}

void check_pointer(void *p) {
    if (p == NULL)
        die("Pointer is NULL.");
}

// Structure to store details for each program
typedef struct program_tag {
    char** argv;    // Array of argument strings
    int argc;       // Argument count
    int pid;        // Process ID
    int fd_in;      // File descriptor for input
    int fd_out;     // File descriptor for output
} Program;

/* Start a specified program without waiting for it to complete.
 *
 * This function performs:
 * - Forking to create a child process
 * - Redirecting stdin/stdout for piping
 * - Upgrading the child process to execute a specified program
 *
 * In case of system call errors, this function exits using die().
 * 
 * Parameters:
 *      programs: array of Program structures
 *      num_programs: total number of programs
 *      cur: index of the current program to start
 *
 * The child process does not return from this function.
 */
void start_program(Program *programs, int num_programs, int cur) {
    pid_t child = fork();

    if (child == -1) {
        die("fork() failed.");
    } else if (child == 0) {
        // In child process
        dup2(programs[cur].fd_in, FD_STDIN);   // Redirect stdin
        dup2(programs[cur].fd_out, FD_STDOUT); // Redirect stdout

        close(programs[cur].fd_in);  // Close unused descriptors
        close(programs[cur].fd_out);

        execvp(programs[cur].argv[0], programs[cur].argv); // Execute program
        die("execvp() failed.");    // If execvp fails
    } else {
        // In parent process
        programs[cur].pid = child;   // Store child's PID

        close(programs[cur].fd_in);  // Close descriptors in parent
        close(programs[cur].fd_out);
    }
}

/* Wait for a specified program to complete.
 *
 * Parameter:
 *      prog: Pointer to Program structure with the program's PID.
 *
 * Return:
 *      -1 if PID is invalid or waitpid() fails,
 *      exit status of the program (0-255) otherwise.
 */
int wait_on_program(Program *prog) {
    int exitStatus;

    if (prog->pid < 0)
        return -1;

    waitpid(prog->pid, &exitStatus, 0);
    return WEXITSTATUS(exitStatus);
}

/* Set up pipes between programs for inter-process communication.
 *
 * Parameters:
 *      programs: Array of Program structures
 *      num_programs: Number of programs
 *
 * This function exits if pipe() fails.
 */
void prepare_pipes(Program *programs, int num_programs) {
    for (int i = 0; i < num_programs - 1; i++) {
        int pd[2];
        if (pipe(pd) == -1) {
            die("pipe() failed.");
        }
        programs[i].fd_out = pd[PIPEFD_WRITE];   // Set output fd
        programs[i + 1].fd_in = pd[PIPEFD_READ];  // Set input fd
    }
}

/*********************************************************/
/* Helper functions - do not modify                      */
/*********************************************************/

// Initialize a Program with space for specified arguments
void init_program(Program *prog, int argc) {
    prog->argv = malloc((argc + 1) * sizeof(char *));
    check_pointer(prog->argv);
    prog->argc = 0;
    prog->argv[0] = NULL;
    prog->pid = prog->fd_in = prog->fd_out = -1;
}

// Release allocated memory for each program
void free_programs(Program *programs, int num_programs) {
    for (int i = 0; i < num_programs; i++) {
        free(programs[i].argv);
        programs[i].argv = NULL;
    }
}

// Parse command line arguments into individual programs
int parse_command_line(Program *progs, int max_num_progs, int argc, char **argv) {
    int cur_arg = 1, prog_count = 0;

    while (cur_arg < argc) {
        if (prog_count == max_num_progs)
            die("Too many programs.");

        init_program(&progs[prog_count], argc);

        int ia = 0;  // Argument index
        while (cur_arg < argc && strcmp(argv[cur_arg], "--")) {
            progs[prog_count].argv[ia++] = argv[cur_arg++];
        }
        progs[prog_count].argv[ia] = NULL;
        progs[prog_count].argc = ia;
        prog_count++;
        if (ia == 0)
            die("Empty program.");

        cur_arg++;  // Skip "--"
        if (cur_arg == argc)
            die("Last program is empty.");
    }
    return prog_count;
}

int main(int argc, char **argv) {
    Program programs[MAX_NUM_PROGRAMS];
    int num_programs;

    if (argc <= 1)
        die("Specify at least one program to run. Separate multiple programs with --");

    num_programs = parse_command_line(programs, MAX_NUM_PROGRAMS, argc, argv);
    prepare_pipes(programs, num_programs);

    for (int i = 0; i < num_programs; i++) {
        fprintf(stderr, "Starting program %d: %s\n", i, programs[i].argv[0]);
        start_program(programs, num_programs, i);
    }

    for (int i = 0; i < num_programs; i++) {
        fprintf(stderr, "Waiting for program %d: %s\n", i, programs[i].argv[0]);
        int status = wait_on_program(&programs[i]);
        fprintf(stderr, "Program %d: %s exited with %d\n", i, programs[i].argv[0], status);
    }

    free_programs(programs, num_programs);

    fprintf(stderr, "Parent: All programs completed successfully.\n");
    return 0;
}
