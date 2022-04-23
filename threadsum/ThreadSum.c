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

long
parallelSum();

long
serialSum();

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

	printf("Parallel sum:  %ld\n", parallelSum());
	printf("Serial sum:    %ld\n", serialSum());

	free(A);
}

void*
thread (void* id)
{
	long sum = 0;
	long block = N / p;
	long start = block * (long)id;
	long end = start + block;

	for(long i = start; i < end; i++)
	{
		sum += A[i];;
	}

	return (void*)sum;
}

long
parallelSum()
{
	pthread_t* threads = malloc(p * sizeof(pthread_t));

	for(long id = 0; id < p; ++id)
	{
		pthread_create(&threads[id], NULL, thread, (void*)id);
	}
	long sum = 0;
	for(long id = 0; id < p; ++id)
	{
		void* returnVal;
		pthread_join(threads[id], &returnVal);
		sum += (long)returnVal;
	}
	free(threads);
	return sum;
}

long
serialSum()
{
	long sum = 0;
	for(int i = 0; i < N; ++i)
	{
		sum += A[i];
	}
	return sum;
}
