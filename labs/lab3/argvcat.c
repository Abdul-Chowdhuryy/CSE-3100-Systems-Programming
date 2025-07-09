#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void my_error(char *s)
{
    perror(s);
    exit(1);
}

char *my_strcat(char *s1, char *s2)
{
    int len1 = strlen(s1);
    int len2 = strlen(s2);
    char *result = (char *)malloc((len1 + len2 + 1) * sizeof(char));
    if (result == NULL) {
        my_error("Memory allocation failed");
    }
    strcpy(result, s1);
    strcat(result, s2);
    return result;
}

int main(int argc, char *argv[])
{
    char *s;
    s = my_strcat("", argv[0]);
    for (int i = 1; i < argc; i++) {
        char *temp = s;
        s = my_strcat(s, argv[i]);
        free(temp);
    }
    printf("%s\n", s);
    free(s);
    return 0;
}
