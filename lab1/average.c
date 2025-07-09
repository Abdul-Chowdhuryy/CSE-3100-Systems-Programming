
#include <stdio.h>

int main(void) {
    double total = 0;
    double average;
    double y;
    int i = 0;

    
    while (scanf("%lf", &y) == 1) {
        total += y;
        i += 1;
        
        average = total / i;

        printf("Total=%f Average=%f\n", total, average);
        
    }
    return 0;

    }



