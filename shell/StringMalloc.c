#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int
main ()
{
  char s[20];
  strcpy (s, "Hello");
  printf ("%s\n", s);

  char* p = strstr (s, "el"); 
  printf ("%s\n", p);

  strcat (s, ", world!"); 
  printf ("%s\n", s);

  char s2[] = "t1  t2,t3  ,     t4   ";
  p = strtok (s2, " ,");
  while (p != NULL)
  {
    printf ("'%s'\n", p);
    p = strtok (NULL, " ,");
  }

  const int N = 8;
  // Note no cast to int* is necessary!
  int* A = malloc (N * sizeof (int));
  for (int i = 0; i < N; ++i)
    A[i] = i;
  for (int i = 0; i < N; ++i)
    printf ("%d ", A[i]);
  printf ("\n");
  free (A);
  
  int* B = malloc (N * sizeof (int));
  for (int i = 0; i < N; ++i)
    printf ("%d ", B[i]);
  printf ("\n");
  free (B);

  int* C = calloc (N, sizeof (int));
  for (int i = 0; i < N; ++i)
    printf ("%d ", C[i]);
  printf ("\n");
  free (C);
  
  return EXIT_SUCCESS;
}
