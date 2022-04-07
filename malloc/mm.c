#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
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
const uint8_t HALF_WORD = WORD_SIZE / 2;
// Add others... 

/****************************************************************/
// Inline functions

/* returns header address given basePtr */
static inline tag* header (address ptr)
{
  return (tag*)ptr - HALF_WORD;
}

/* Returns true if the block is allocated */
static inline bool isAllocated (address ptr)
{
  return *header(ptr) & 1;
}

/* Returns size of block */ 
static inline tag sizeOf (address ptr)
{
  return (*header(ptr) >> 1) << 1;
}

/* returns footer address given basePtr */
static inline tag* footer (address ptr)
{
  return (tag*)ptr + (sizeOf(ptr) * WORD_SIZE) - WORD_SIZE;
}

/* gives the basePtr of next block */
static inline address nextBlock (address ptr)
{
  return ptr + (sizeOf(ptr) * WORD_SIZE);
}

/* returns the pointer to the prev blocks footer */
static inline tag* prevFooter (address ptr)
{
  return (tag*)ptr - WORD_SIZE;
}


/* returns the pointer to the next blocks header */
static inline tag* nextHeader (address ptr)
{
  return header(nextBlock(ptr));
}

/* gives the basePtr of prev block*/
static inline address prevBlock (address ptr)
{
  return ptr - ((*prevFooter(ptr) >> 1) << 1) * WORD_SIZE;
}


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


