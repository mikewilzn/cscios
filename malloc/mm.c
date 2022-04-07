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
const uint8_t TAG_SIZE = sizeof (tag);
const uint8_t DWORD_SIZE = WORD_SIZE * 2;
// Add others... 

/****************************************************************/
// Inline functions

/* returns header address given basePtr */
static inline tag* header (address ptr)
{
  return (tag*) (ptr - TAG_SIZE);
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

int
main ()
{
  // Each line is a DWORD
  //        Word      0       1
  //                 ====  ===========
  /* //tag heapZero[] = { 0, 0, 1, 4 | 1,
  		     0, 0, 0, 0,
  		     0, 0, 4 | 1, 2 | 0,
  		     0, 0, 2 | 0, 1 };  */
  tag heapZero[16] = { 0 }; 
  // Point to DWORD 1 (DWORD 0 has no space before it)
  address g_heapBase = (address) heapZero + DWORD_SIZE;
  makeBlock (g_heapBase, 6 , 0);
  *prevFooter (g_heapBase) = 0 | 1;
  *nextHeader (g_heapBase) = 1;
  //makeBlock (g_heapBase, 4 , 1);
  //makeBlock (nextBlock (g_heapBase), 2, 0); 
  printPtrDiff ("header", header (g_heapBase), heapZero);
  printPtrDiff ("footer", footer (g_heapBase), heapZero);
  printPtrDiff ("nextBlock", nextBlock (g_heapBase), heapZero);
  printPtrDiff ("prevFooter", prevFooter (g_heapBase), heapZero);
  printPtrDiff ("nextHeader", nextHeader (g_heapBase), heapZero);
  address twoWordBlock = nextBlock (g_heapBase); 
  printPtrDiff ("prevBlock", prevBlock (twoWordBlock), heapZero);

  printf ("%s: %d\n", "isAllocated", isAllocated (g_heapBase)); 
  printf ("%s: %d\n", "sizeOf", sizeOf (g_heapBase));

  // Canonical loop to traverse all blocks
  printf ("All blocks\n"); 
  for (address p = g_heapBase; sizeOf (p) != 0; p = nextBlock (p))
    printBlock (p);
  
  return 0;
}
