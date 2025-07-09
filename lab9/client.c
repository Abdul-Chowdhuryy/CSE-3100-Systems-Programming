/*
 ** client.c -- a stream socket client demo
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <time.h>

#include <arpa/inet.h>

#define PORT "3119" // the port client will be connecting to 

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

#include "socketio.c"

#define	LINE_SIZE	100

//_TMPL CUT
void die(char *s)
{
    if (errno)
        perror(s);
    else 
        fprintf(stderr, "Error: %s\n", s);
    exit(EXIT_FAILURE);
}

void print_line(int sid, char * buf, int n)
{
    if (recv_lines(sid, buf, n)) {
        fprintf(stderr, "recv_lines() returned -1.\n");
        exit(EXIT_FAILURE);
    }
    printf("%s", buf);
}

int send_int(int sid, int v)
{
    char buf[64];
    int n;

    n = snprintf(buf, sizeof(buf), "%d\n", v);
    return send_all(sid, buf, n);
}

int get_number(char *s, int *pv)
{
    if (sscanf(s, "%d\n", pv) != 1)
        return -1;
    return 0;
}
//_TMPL END

int main(int argc, char *argv[])
{
    int sockfd;
    char buf[LINE_SIZE];
    struct addrinfo hints, *servinfo, *p;
    int rv;
    char s[INET6_ADDRSTRLEN];

    if (argc != 2) {
        fprintf(stderr,"usage: client hostname\n");
        exit(1);
    }

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if ((rv = getaddrinfo(argv[1], PORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and connect to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                        p->ai_protocol)) == -1) {
            perror("client: socket");
            continue;
        }

        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            perror("client: connect");
            close(sockfd);
            continue;
        }

        break;
    }

    if (p == NULL) {
        fprintf(stderr, "client: failed to connect\n");
        return 2;
    }

    inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
            s, sizeof s);
    printf("client: connecting to %s\n", s);

    freeaddrinfo(servinfo); // all done with this structure

    int min = 1, max, guess;
    int result = 0;

    /* TODO
     *
     *  get max
     *  repeat the following until result is 0
     *      calculate guess
     *      send guess 
     *      receive result
     *      adjust min or max if necessary
     *  print the final message, which may or may not be received with "0\n"
     *  */

    // get the max from the server and print it into the buf array
    print_line(sockfd, buf, LINE_SIZE);

    // extract the number by using get_number
    if (get_number(buf, &max)) {
        // error message if it does not work
        fprintf(stderr, "get_number() returned -1.\n");
        exit(EXIT_FAILURE);
    }

    // do while loop to repeat until the result is 0
    do {
        // uses binary search to do so
        guess = (min + max) / 2;

        // sends the the guess to the server
        if (send_int(sockfd, guess)) {
            // error handling
            fprintf(stderr, "send_int() returned -1.\n");
            exit(EXIT_FAILURE);
        }

        // prints out the guess
        printf("My guess: %d\n", guess);

        // gets the resutl back from the server
        print_line(sockfd, buf, LINE_SIZE);
        // extract the result number by using get_number()
        if (get_number(buf, &result)) {
            // error handling
            fprintf(stderr, "get_number() returned -1.\n");
            exit(EXIT_FAILURE);
        }

        // if result is 1, then increase the min
        if (result == 1)
            min = guess + 1;
        // if the result is -1, then decrease the max
        else if (result == -1)
            max = guess - 1;
        

    } while (result != 0);

    // print out final line stating the attemps and number that was guessed
    print_line(sockfd, buf, LINE_SIZE);

    close(sockfd);
    return 0;
}
