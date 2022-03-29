#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>

const int ARG_NUM = 3;

int
main (int argc, char* argv[])
{
    if (argc != ARG_NUM)
    {
        printf("Correct syntax is: %s SOURCE DESTINATION\n", argv[0]);
        exit(0);
    }

    if ((open(argv[1], O_RDONLY)) < 0)
    {
        fprintf(stderr, "Cannot open file \"%s\"\n", argv[1]);
        exit(1);
    }
    int destFile = open(argv[2], O_CREAT, O_WRONLY, O_TRUNC, 0600);

    return 0;
}