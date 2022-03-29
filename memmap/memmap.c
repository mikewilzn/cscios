#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>
#include <string.h>

const int ARG_NUM = 3;

int
main (int argc, char* argv[])
{
    if (argc != ARG_NUM)
    {
        printf("Correct syntax is: %s SOURCE DESTINATION\n", argv[0]);
        exit(EXIT_SUCCESS);
    }

    int srcFile = open(argv[1], O_RDONLY);
    if (srcFile < 0)
    {
        fprintf(stderr, "Cannot open file \"%s\"\n", argv[1]);
        exit(EXIT_FAILURE);
    }

    int destFile = open(argv[2], O_CREAT | O_RDWR | O_TRUNC, 0600);
    if(destFile < 0)
    {
        perror("Cannot open output file\n");
        exit(EXIT_FAILURE);
    }

    struct stat info;
    fstat(srcFile, &info);

    if ((ftruncate(destFile, info.st_size)) < 0)
    {
        perror("ftruncate() error");
        exit(EXIT_FAILURE);
    }

    void* src = mmap(NULL, info.st_size, PROT_READ, MAP_SHARED, srcFile, 0);
    void* dest = mmap(NULL, info.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, destFile, 0);

    memcpy(dest, src, info.st_size);

    munmap(src, info.st_size);
    munmap(dest, info.st_size);

    close(srcFile);
    close(destFile);

    return 0;
}