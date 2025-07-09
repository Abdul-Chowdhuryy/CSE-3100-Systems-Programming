#include <stdio.h>
#include <stdlib.h>
#include <string.h>

double simulate_2d_random_walk(int n);

int main(int argc, char* argv[]) {
    int trials = 1000;
    int seed = (argc == 2) ? atoi(argv[1]) : 12345;
    srand(seed);

    for (int n = 1; n <= 64; n *= 2) {
        double sum = 0.0;
        for (int i = 0; i < trials; i++) {
            double p = simulate_2d_random_walk(n);
            sum += p;
        }
        printf("%d %.3lf\n", n, sum / trials);
    }
    return 0;
}

double simulate_2d_random_walk(int n) {
    int x = 0, y = 0;
    int visited[2 * n + 1][2 * n + 1];
    memset(visited, 0, sizeof(visited));  // Clear the visited array

    int visited_count = 0;

    while (x > -n && x < n && y > -n && y < n) {
        if (!visited[x + n][y + n]) {
            visited[x + n][y + n] = 1;
            visited_count++;
        }

        // Randomly choose direction: 0=up, 1=right, 2=down, 3=left
        int direction = rand() % 4;
        switch (direction) {
            case 0: y++; break; // up
            case 1: x++; break; // right
            case 2: y--; break; // down
            case 3: x--; break; // left
        }
    }

    int total_points = (2 * n - 1) * (2 * n - 1);
    return (double)visited_count / total_points;
}
