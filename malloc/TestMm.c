// Gary M. Zoppetti
// Testing framework for the Malloc Lab

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
  // This initializes the heap to look like the sixth diagram
  //   in the guide. 
  // Each line is a DWORD
  //        Word      0       1
  //                 ====  ===========
  tag heapZero[] = { 0, 0, 0 | 1, 4 | 1,
  		     0, 0, 0, 0,
  		     0, 0, 4 | 1, 2 | 0,
  		     0, 0, 2 | 0, 0 | 1 }; 
  //tag heapZero[16] = { 0 }; 
  // Point to DWORD 1 (DWORD 0 has no space before it)
  address g_heapBase = (address) heapZero + DWORD_SIZE;
  //makeBlock (g_heapBase, 6 | 0);
  //*prevFooter (g_heapBase) = 0 | 1;
  //*nextHeader (g_heapBase) = 1;
  //makeBlock (g_heapBase, 4 | 1);
  //makeBlock (nextBlock (g_heapBase), 2); 
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

//static inline address
//extendHeap (uint32_t numWords)
//{
  // Use mem_sbrk to increase the heap size by the appropriate #
  //   of bytes. Return NULL on error.
  // Make a free block of size "numWords"
  // Place the end sentinel header
  // Coalesce newly created free block
//}

/***********************************************************************/

/*
  Allocating initial heap area and default-initializing any global variables
  Place sentinel blocks in heap, one in prologue and one in epilogue
  Return -1 if any problems, else 0
*/
//int
//mm_init (void)
//{
  // Use mem_sbrk to allocate a heap of some # of DWORDS, perhaps 4
  //   like in the guide (at least 1 DWORD). 
  // Return -1 on error.
  // Set g_heapBase to point to DWORD 1 (0-based). 
  // Make a free block.
  // Place the begin sentinel footer
  // Place the end sentinel header
  // Return 0 for success
//}
