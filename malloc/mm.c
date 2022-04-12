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
  bool nextAlloc = isAllocated(nextBlock(ptr));
  
  uint32_t size = sizeOf(ptr);
  
  if (prevAlloc && nextAlloc) // Both adjacent blocks are allocated
    return ptr;
  else if (!prevAlloc && nextAlloc) // Previous block is not allocated
  {
    address prev = prevBlock(ptr);
    size += sizeOf(prev); // Combine size of current block and previous block
    makeBlock(prev, size, false);
    return prev;
  }
  else if (prevAlloc && !nextAlloc) // Next block is not allocated
  {
    size += sizeOf(nextBlock(ptr));
    makeBlock(ptr, size, false);
    return ptr;
  }
  else // Both adjacent blocks are not allocated
  {
    address prev = prevBlock(ptr);
    size += (sizeOf(prev) + sizeOf(nextBlock(ptr)));
    makeBlock(prev, size, false);
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

  while(sizeOf(ptr) != 0) // checks if block will fit 
  {
    if (!isAllocated(ptr))
    {
      tag ptrSize = sizeOf(ptr);
      if (ptrSize - numWords >= MIN_BLOCK_SIZE)
      {
        makeBlock(ptr, numWords, true);
        makeBlock(nextBlock(ptr), ptrSize - numWords, false);
        return ptr;
      }
      else if (ptrSize == numWords)
      {
        toggleBlock(ptr);
        return ptr;
      }
      ptr = extendHeap(numWords);
      if (ptr == NULL)
        return NULL;
      toggleBlock(ptr);
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
printPtrDiff (const char* header, void* p, void* base)
{
  printf ("%s: %td\n", header, (address) p - (address) base);
}

void
printBlock (address p)
{
  printf ("Block Addr %p; Size %u; Alloc %d\n",
	  p, sizeOf (p), isAllocated (p)); 
}



