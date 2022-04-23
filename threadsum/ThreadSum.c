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
int* g_A; // Array of integers
int g_N; // Number of elements in array g_A
int g_p; // Number of threads to spawn for parallel computation

/******* Function prototypes *******/
void*
thread (void* id);

long
parallelSum();

long
serialSum();

/******* Main *******/

int
main (int argc, char* argv[])
{
	printf("p ==> ");
	scanf("%d", &g_p);
	printf("N ==> ");
	scanf("%d", &g_N);

	g_A = malloc(g_N * sizeof(int));

	for(int i = 0; i < g_N; i++)
	{
		g_A[i] = rand() % 5; // mod 5 gives range [0,4]
	}

	printf("Parallel sum:  %ld\n", parallelSum());
	printf("Serial sum:    %ld\n", serialSum());

	free(g_A);
}

/*
 * thread()
 * @params - id - Thread id
 * @return - Sum of integers in the thread's block of elements
 */
void*
thread (void* id)
{
	long sum = 0;
	long block = g_N / g_p;
	long start = block * (long)id;
	long end = start + block;

	for(long i = start; i < end; i++)
	{
		sum += g_A[i];;
	}

	return (void*)sum;
}

/*
 * parallelSum() = Compute sum of ints in array g_A using parallel threads
 * @params - none
 * @return - sum of values in array
 */
long
parallelSum()
{
	pthread_t* threads = malloc(g_p * sizeof(pthread_t));

	for(long id = 0; id < g_p; ++id)
	{
		pthread_create(&threads[id], NULL, thread, (void*)id);
	}
	long sum = 0;
	for(long id = 0; id < g_p; ++id)
	{
		void* returnVal;
		pthread_join(threads[id], &returnVal);
		sum += (long)returnVal;
	}
	free(threads);
	return sum;
}

/*
 * serialSum() - Compute sum of ints in array g_A using only the main thread
 * @params - none
 * @return - sum of values in array
 */
long
serialSum()
{
	long sum = 0;
	for(int i = 0; i < g_N; ++i)
	{
		sum += g_A[i];
	}
	return sum;
}
