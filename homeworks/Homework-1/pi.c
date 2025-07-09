#include <stdio.h>
#include <stdlib.h>

int main(void) {
    int n, i;

    // Add a space after "n ="
    printf("n = ");
    scanf("%d", &n);

    double pi = 0;
    double power_16 = 1.0;
    double total;

    for (i = 0; i <= n; i += 1) {
        total = (4.0 / (8 * i + 1) - 2.0 / (8 * i + 4) - 1.0 / (8 * i + 5) - 1.0 / (8 * i + 6)) * power_16;
        pi += total;
        power_16 /= 16;
    }

    
    printf("PI = %.10f\n", pi);

    return 0;
}
