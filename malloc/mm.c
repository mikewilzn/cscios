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

  if (prevAlloc)
    return ptr;
  
  if(!prevAlloc && ptr == g_heapBase)
  {
    address prev = prevBlock(ptr);
    makeBlock(prev, sizeOf(prev) + sizeOf(ptr), false);
    return prev;
  }
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
  
  while (sizeOf(ptr) != 0)
  {
    if(!isAllocated(ptr))
    {
      if (sizeOf(ptr) - numWords >= MIN_BLOCK_SIZE)
      {
        // split the block if size is greater than current block
        tag oldSize = sizeOf(ptr);
        makeBlock(ptr, numWords, true);
        makeBlock(nextBlock(ptr), oldSize - numWords, false);
        return ptr;
      }
      if (sizeOf(ptr) == numWords)
      {
        makeBlock(ptr, numWords, true);
        return ptr;
      }
    }
    ptr = nextBlock(ptr);
  }

  /* If we reach end of heap without finding a free block */
  ptr = extendHeap(numWords);
  if (ptr == NULL)
    return NULL;
  makeBlock(ptr, numWords, true);
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
  /* the words needed for new block */
  uint32_t newWords = align(size);
  /* if null malloc the size */
  if (ptr == NULL)
  {
    return mm_malloc(size);
  }
  /* if size 0 then free */
  if (size == 0)
  {
    mm_free(ptr);
    return NULL;
  }

  /* store pointer to original payload */
  address original = ptr;
  uint32_t originalSize = sizeOf(original);
  /* check next block for free to save space */
  if (!isAllocated(nextBlock(original)))
  {
    originalSize += sizeOf(nextBlock(original));
  }
  /*check current block for free to save space */
  if (!isAllocated((address)header(original)))
  {
    ptr = prevBlock(original);
    originalSize += sizeOf(ptr);
  }
  /* if original size is less than size getting placed, place the block */
  if (newWords <= originalSize)
  {
    /* copy data into ptr so the pointer can be placed */
    memcpy(ptr, original, sizeOf(original) * WORD_SIZE - WORD_SIZE);
    makeBlock(ptr, newWords, 1);
    /*check for empty space, if free space exists then free the remaining blocks*/
    if ((int)(originalSize - newWords) >= 0)
    {
      makeBlock(nextBlock(ptr), originalSize - newWords, 0);
    }
    return ptr;
  }
  /* malloc a new block when no space can be found */
  address newBlock = mm_malloc(size);
  /*copy the data from old block to new block*/
  memcpy(newBlock, original, originalSize * WORD_SIZE - WORD_SIZE);
  mm_free(original);
  /*return the newblock's base pointer*/
  return newBlock;
}

void
printBlock (address ptr)
{
  printf("Block Addr %p; Size %u; Alloc %d\n", ptr, sizeOf(ptr), isAllocated(ptr));
}

int
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
}

int mm_check()
{
    // canonical loop to traverse all blocks
    printf("all blocks\n");
    for(address p = g_heapBase; sizeOf(p) !=0; p = nextBlock(p)){
        printBlock(p);
    }
    return 0;
}



