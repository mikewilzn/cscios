/*
  Filename: mm.c
  Author: Mike Wilson and Tanner Michael
  Course: 380
  Assignment: malloc
  Description: This file implements malloc, free, realloc using a list and coalesce on free. 
  The size is in words and we are making sure to align on the correct word
  If freeblock found is bigger than the size needed (malloc) split the free block 
  into the size needed and then a free portion afterward. 
*/

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
const uint8_t DWORD_SIZE = WORD_SIZE * 2;
const uint8_t ALIGNMENT = DWORD_SIZE;
const uint8_t TAG_SIZE = sizeof (tag);
const uint8_t MIN_BLOCK_SIZE = 2;

/****************************************************************/
// Private global variables
static address g_heapBase;

int
mm_check();
/****************************************************************/
// Inline functions

/* returns header address given basePtr */
static inline tag* header (address ptr)
{
  return (tag*) (ptr - sizeof(tag));
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
  return (tag*) (ptr + (sizeOf(ptr) * WORD_SIZE) - WORD_SIZE);
}

/* gives the basePtr of next block */
static inline address nextBlock (address ptr)
{
  return ptr + (sizeOf(ptr) * WORD_SIZE);
}

/* returns the pointer to the prev blocks footer */
static inline tag* prevFooter (address ptr)
{
  return (tag*) (ptr - WORD_SIZE);
}


/* returns the pointer to the next blocks header */
static inline tag* nextHeader (address ptr)
{
  return header(nextBlock(ptr));
}

/* gives the basePtr of prev block*/
static inline address prevBlock (address ptr)
{
  return ptr - (((*prevFooter(ptr) >> 1) << 1) * WORD_SIZE);
}

/* basePtr, size, allocated */
static inline void makeBlock (address ptr , uint32_t size, bool allocated)
{
  *header(ptr) = size | allocated;
  *footer(ptr) = size | allocated;
}

/* basePtr - toggles allocated/free */
static inline void toggleBlock (address ptr)
{
  *header(ptr) ^= 1;
  *footer(ptr) ^= 1;
}

static inline address coalesce (address ptr)
{
  /* Get previous and next block allocation status */
  bool prevAlloc = *prevFooter(ptr) & (tag)1; // Can't use prevBlock since dummy footer is of size zero
  //bool nextAlloc = isAllocated(nextBlock(ptr));
  if(!prevAlloc && ptr == g_heapBase)
  {
    address prev = prevBlock(ptr);
    makeBlock(prev, sizeOf(prev) + sizeOf(ptr), false);
    return prev;
  }
  return ptr;
}

/* 
 * Increase heap size by given number of bytes 
 * Returns base pointer of the new block
 * or NULL if there is an error
 */
static inline address extendHeap (uint32_t numWords)
{
  address ptr = mem_sbrk ((int)numWords * WORD_SIZE);
  if (ptr == NULL)
    return NULL;

  makeBlock (ptr, numWords, false);
  *nextHeader(ptr) = 0 | true; // true = allocated bit 1
  return coalesce(ptr);
}

/* Calculates number of words needed for proper alignment given a particular size */
static inline uint32_t align (uint32_t size)
{
  uint32_t numWords = size + (2 * TAG_SIZE);
  numWords = (numWords + (DWORD_SIZE - 1)) / DWORD_SIZE;
  numWords = numWords * 2;
  return numWords;
}

/****************************************************************/
// Non-inline functions

int
mm_init (void)
{
  address ptr = mem_sbrk (4 * DWORD_SIZE);
  if (ptr == NULL)
    return -1;
  
  g_heapBase = ptr + ALIGNMENT; // Moves base pointer to double word alignment
  
  /* Create dummy header and footer */
  *prevFooter(g_heapBase) = (0 | true);
  *nextHeader(g_heapBase) = (0 | true);

  makeBlock(g_heapBase, 6, 0);

  return 0;
}

/****************************************************************/

void*
mm_malloc (uint32_t size)
{
  address ptr = g_heapBase;
  uint32_t numWords = align(size);

  if (size == 0)
    return NULL;
  // loop through heap
  for (address ptr = g_heapBase; sizeOf(ptr) == 0; ptr = nextBlock(ptr))
  {
    //if allocated or too small continue looking
    if (isAllocated(ptr))
    {
      continue;
    }
    // still not big enough
    else if (sizeOf(ptr) < numWords)
    {
      continue;
    }
    //found adequate block
    else if (sizeOf(ptr) == size)
    {
      makeBlock(ptr, numWords, true);
      return ptr;
    }
    // size is greater than curr block so split block 
    else if (sizeOf(ptr) > size)
    {
      tag oldSize = sizeOf(ptr);
      makeBlock(ptr, numWords, true);
      makeBlock(nextBlock(ptr), oldSize - numWords, false);
      return ptr;
    }
  }
  // end of heap reached 
  ptr = extendHeap(numWords);
  tag ptrSize = sizeOf(ptr);
  //if extended heap matches required size then place it 
  if (ptrSize == numWords)
  {
    makeBlock(ptr, numWords, true);
  }
  // place the block and if extra space, free the rest
  else
  {
    makeBlock(ptr, numWords, true);
    makeBlock(nextBlock(ptr), ptrSize - numWords, false);
  }
  return ptr;
}

/****************************************************************/

/* sets the specified block to not allocated and tries to coalesce with 
  next and previous blocks to make more free space */
void
mm_free (void *ptr)
{
  toggleBlock(ptr);
  coalesce(ptr);
}

/****************************************************************/

void*
mm_realloc (void *ptr, uint32_t size)
{
  if (ptr == NULL)
  {
    ptr = mm_malloc(size);
    return ptr;
  }

  if (size == 0)
  {
    mm_free(ptr);
    return ptr;
  }

  address tempPtr = mm_malloc(size);
  tempPtr = memcpy(tempPtr, ptr, sizeOf(ptr) * WORD_SIZE - WORD_SIZE);
  mm_free(ptr);
  return tempPtr;
}

void
printBlock (address ptr)
{
  printf("Block Addr %p; Size %u; Alloc %d\n", ptr, sizeOf(ptr), isAllocated(ptr));
}

/* int
main ()
{
  mem_init();
  mm_init();
  mm_malloc(22);
  mm_check();
  mm_malloc(26);
  mm_check();
  mm_malloc(4);
  mm_check();
} */

int mm_check()
{
    // canonical loop to traverse all blocks
    printf("all blocks\n");
    for(address p = g_heapBase; sizeOf(p) !=0; p = nextBlock(p)){
        printBlock(p);
    }
    return 0;
}



