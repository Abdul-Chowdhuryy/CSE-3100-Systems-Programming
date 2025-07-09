#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <ctype.h>
#include <assert.h>

#define BUFFER_SIZE 10240
#define DICT_WORD_COUNT 60000
#define WORD_MAX_LENGTH 80

char dictionary[DICT_WORD_COUNT][WORD_MAX_LENGTH];
int total_words = 0;

void load_dictionary(char *filename)
{
    FILE *file;
    file = fopen(filename, "r");
    if(file == NULL)
    {
        printf("Cannot open file %s.\n", filename);
        exit(-1);
    }
    while(!feof(file))
        fscanf(file, "%s\n", dictionary[total_words++]);
    fclose(file);
}

int is_in_dictionary(char *word)
{
    int left = 0;
    int right = total_words - 1;

    while (left <= right) {
        int mid = (right + left) / 2;
        int cmp_result = strcmp(dictionary[mid], word);
        if (cmp_result == 0) {
            return 1;
        }
        if (cmp_result < 0) {
            left = mid + 1;
        }
        else {
            right = mid - 1;
        }
    }
    return 0;
}

void decrypt_message(unsigned char key, unsigned char shift, const int *encoded_message, int length, char *decoded_message)
{
    for(int i = 0; i < length; i++)
    {
        decoded_message[i] = (char)((encoded_message[i] ^ key) >> shift);
    }
    decoded_message[length] = '\0';
}

int calculate_message_score(const char *message)
{
    char temp_message[BUFFER_SIZE];
    strcpy(temp_message, message);
    char *token = strtok(temp_message, " ");
    int score = 0;

    while(token != NULL){
        if (is_in_dictionary(token)){
            score++;
        }
        token = strtok(NULL, " ");
    }

    return score;
}

void find_original_message(const int *encoded_message, int length, char *result_message)
{
    char decoded_message[BUFFER_SIZE];
    int highest_score = 0;
    strcpy(result_message, "");
    for(unsigned char key = 0; key < 255; key++)
    {
        for(unsigned char shift = 0; shift <= 24; shift++)
        {
            decrypt_message(key, shift, encoded_message, length, decoded_message);
            int current_score = calculate_message_score(decoded_message);
            if(current_score > highest_score)
            {	
                highest_score = current_score;
                strcpy(result_message, decoded_message);
            }
        }
    }
}

int read_encoded_message(char *filename, int *encoded_message)
{
    int file_desc = open(filename, O_RDONLY);
    if(file_desc < 0)
    {
        printf("Cannot open the file\n");
        return -1;
    } 
    size_t bytes_read = read(file_desc, encoded_message, sizeof(int) * BUFFER_SIZE);
    close(file_desc);
    return (bytes_read / sizeof(int));
}

int main(int argc, char *argv[])
{
    if(argc != 2)
    {
        printf("%s encrypted-message\n", argv[0]);
        return 0;
    }

    load_dictionary("dict.txt");
    int encoded_message[BUFFER_SIZE];
    int length = read_encoded_message(argv[1], encoded_message);
    char result_message[BUFFER_SIZE];
    strcpy(result_message, "");
    find_original_message(encoded_message, length, result_message);
    printf("%s\n", result_message);
    return 0;
}


