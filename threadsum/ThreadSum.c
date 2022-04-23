/*
 * Filename	: ThreadSum.c
 * Author	: Mike Wilson
 * Course	: CSCI 380-01
 * Assignment	: Threads
 * Description	: Write a Pthreads program that computes a parallel
 * sum of an array A of N random integers in the range [0, 4]
 */

#include <stdio.h>
#include <stdlib.h>

#include <pthread.h>

/******* Global Variables *******/
int* A;
int N;
int p;

/******* Function prototypes *******/
void*
thread (void* id);

int
main (int argc, char* argv[])
{
	printf("p ==> ");
	scanf("%d", &p);
	printf("N ==> ");
	scanf("%d", &N);

	A = malloc(N * sizeof(int));

	for(int i = 0; i < N; i++)
	{
		A[i] = rand() % 5; // mod 5 gives range [0,4]
	}

	pthread_t* threads = malloc(p * sizeof(pthread_t));
}

void*
thread (void* id)
{
	long sum = 0;
	int block = N / p;
	int start = block * (int)id;
	int end = start + block;

	for(int i = start; i < end; i++)
	{
		sum += A[i];;
	}

	return (void*)sum;
}
