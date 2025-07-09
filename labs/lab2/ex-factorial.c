#include <stdio.h>

int factorial(int n)
{
    if (n == 0)
        return 1; // 
    return n * factorial(n - 1); 
}

int main()
{
    int n, result;

    if (scanf("%d", &n) != 1) 
        return 1; 

    if (n < 0) {
        printf("Factorial is not defined for negative numbers.\n");
        return 1; 
    }

    result = factorial(n);
    printf("%d\n", result);
    return 0;
}
