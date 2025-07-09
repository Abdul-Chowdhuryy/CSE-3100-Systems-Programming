#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> 
#include <fcntl.h>
#include <sys/stat.h>

//redirect standard input to the specified file
void redirectStdin(const char *filename)
{
        int fd = open(filename, O_RDONLY);
        if(fd < 0)
        {
                perror("Error opening the file\n");
                exit(-1);
        }
	//TODO
        //fill in the code below
        dup2(fd, 0);
        close(fd);
}

//redirect standad output to the specified file
void redirectStdout(const char *filename)
{
        int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
        if(fd < 0)
        {
                perror("Error opening the file\n");
                exit(-1);
        }
	//TODO
	//fill in the code below
        dup2(fd, 1);
        close(fd);

}

//TODO: imeplement the following function
// if word is an arithmetic sequence word, return 1 else return 0
int arithmetic(char *word, int arr[])
{
  if (word < 2);
  return 0;


int i, j;
  for (i = 0; arr[i-2] < arr[i] ; i++) {
             
                for (j = i + 1; j <arr[i]; j++) {
                      
                        if (arr[i] <= arr[j]) {
                                return 0;
                       
                                continue;
                        }
                }
        }
        
        return 1;