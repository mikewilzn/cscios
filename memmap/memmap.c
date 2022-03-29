/*
 * Filename	: memmap.c
 * Author	: Michael Wilson
 * Course	: CSCI 380-01
 * Assignment	: Memory Mapping
 * Description	: Implement cp via the mmap function
 */

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

    /* Open source file read-only */
    int srcFile = open(argv[1], O_RDONLY);
    if (srcFile < 0)
    {
        fprintf(stderr, "Cannot open file \"%s\"\n", argv[1]);
        exit(EXIT_FAILURE);
    }

    /* Open destination file. Creating new file in read/write mode with correct permissions. */
    int destFile = open(argv[2], O_CREAT | O_RDWR | O_TRUNC, 0600);
    if (destFile < 0)
    {
        perror("Cannot open output file\n");
        exit(EXIT_FAILURE);
    }

    /* Retrieves stats on source file. Looking for size of source file. */
    struct stat info;
    fstat(srcFile, &info);

    /* Sets size of destination file to that of the source file */
    if ((ftruncate(destFile, info.st_size)) < 0)
    {
        perror("ftruncate() error");
        exit(EXIT_FAILURE);
    }

    /*
     * Maps address space of source file.
     * PROT_READ so mapping can be read.
     * MAP_SHARED so mapping can be copied to destination file.
     */ 
    void* src = mmap(NULL, info.st_size, PROT_READ, MAP_SHARED, srcFile, 0);
    if (src == MAP_FAILED)
    {
        perror("mmap() error");
        exit(EXIT_FAILURE);
    }

    /* Maps address space of destination file.
     * PROT_WRITE so address space can be written to.
     */
    void* dest = mmap(NULL, info.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, destFile, 0);
    if (dest == MAP_FAILED)
    {
        perror("mmap() error");
        exit(EXIT_FAILURE);
    }

    /* Copies address space of source file to destination file */
    memcpy(dest, src, info.st_size);

    /* Unmaps source and destination files from memory */
    munmap(src, info.st_size);
    munmap(dest, info.st_size);

    close(srcFile);
    close(destFile);

    return 0;
}