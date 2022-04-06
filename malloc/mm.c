#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "memlib.h"
#include "mm.h"

/****************************************************************/
// Useful type aliases

typedef uint64_t word;
typedef uint32_t tag;
typedef uint8_t  byte;
typedef byte*    address; 

/****************************************************************/
// Useful constants

const uint8_t WORD_SIZE = sizeof (word);
// Add others... 

/****************************************************************/
// Inline functions


/****************************************************************/
// Non-inline functions

int
mm_init (void)
{
  return 0;
}

/****************************************************************/

void*
mm_malloc (uint32_t size)
{
  fprintf(stderr, "allocate block of size %u\n", size);
  return NULL;
}

/****************************************************************/

void
mm_free (void *ptr)
{
  fprintf(stderr, "free block at %p\n", ptr);
}

/****************************************************************/

void*
mm_realloc (void *ptr, uint32_t size)
{
  fprintf(stderr, "realloc block at %p to %u\n", ptr, size);
  return NULL;
}
