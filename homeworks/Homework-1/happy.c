#include <stdio.h>
#include <stdlib.h>

int main()
{
    int n;
    scanf("%d", &n);

    int m = n;

    while (m != 1 && m != 4) {
        int sum = 0;
        int temp = m;

        while (temp > 0) {
            int digit = temp % 10;
            sum += digit * digit;
            temp /= 10;
        }

        m = sum;

        if (m != 1 && m != 4) {
            printf("n = %d\n", m);
            break;
        }
    }

    return 0;
}
