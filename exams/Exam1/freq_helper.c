#include <assert.h>

//TODO

unsigned firstDigit(unsigned k) {
    while (k >= 10) {
        k /= 10;
    }
    return k;
}

//TODO
void FirstDigitCount(unsigned *a, unsigned n, unsigned freq[10])
{
	 for (unsigned i = 0; i < n; i++) {
        unsigned firstdigit = firstDigit(a[i]);
        freq[firstdigit]++;
    }
}