#include "fs.h"
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>

#define FS_NUM_BLOCKS    128
#define FS_MAX_FILE_SIZE 8
#define FS_MAX_FILENAME  16
#define FS_MAX_FILES     16
#define FS_BLOCK_SIZE    1024

struct fs_t
{
	int fd;
};

struct inode
{
	char name[16];		// file name
	int size;		// file size (in number of blocks)
	int blockPointers[8];	// direct block pointers
	int used;		// 0 => inode is free; 1 => in use
};

// open the file with the above name
void
fs_open (struct fs_t *fs, char diskName[16])
{
  // this file will act as the "disk" for your file system
  if ((fs->fd = open(diskName, O_RDWR)) < 0)
  {
	  printf("Error opening filesystem\n");
	  exit(1);
  }
  printf("Disk [ %s ] opened successfully\n", diskName);
}

// close and clean up all associated things
void
fs_close (struct fs_t *fs)
{
  // this file will act as the "disk" for your file system
  if (close(fs->fd) < 0)
  {
	  printf("Error closing filesystem\n");
	  exit(1);
  }
  printf("Filesystem closed successfully\n");
}

// create a file with this name and this size
void
fs_create (struct fs_t *fs, char name[16], int size)
{

  // high level pseudo code for creating a new file

  // Step 1: check to see if we have sufficient free space on disk by
  // reading in the free block list. To do this:
  // - move the file pointer to the start of the disk file.
  // - Read the first 128 bytes (the free/in-use block information)
  // - Scan the list to make sure you have sufficient free blocks to
  //   allocate a new file of this size

  // Step 2: we look  for a free inode on disk
  // - Read in a inode
  // - check the "used" field to see if it is free
  // - If not, repeat the above two steps until you find a free inode
  // - Set the "used" field to 1
  // - Copy the filename to the "name" field
  // - Copy the file size (in units of blocks) to the "size" field

  // Step 3: Allocate data blocks to the file
  // - Scan the block list that you read in Step 1 for a free block
  // - Once you find a free block, mark it as in-use (Set it to 1)
  // - Set the blockPointer[i] field in the inode to this block number.
  // - repeat until you allocated "size" blocks

  // Step 4: Write out the inode and free block list to disk
  // - Move the file pointer to the start of the file
  // - Write out the 128 byte free block list
  // - Move the file pointer to the position on disk where this inode was stored
  // - Write out the inode
}

// Delete the file with this name
void
fs_delete (struct fs_t *fs, char name[16])
{

  // Step 1: Locate the inode for this file
  //   - Move the file pointer to the 1st inode (129th byte)
  //   - Read in a inode
  //   - If the iinode is free, repeat above step.
  //   - If the iinode is in use, check if the "name" field in the
  //     inode matches the file we want to delete. IF not, read the next
  //     inode and repeat

  // Step 2: free blocks of the file being deleted
  // Read in the 128 byte free block list (move file pointer to start
  //   of the disk and read in 128 bytes)
  // Free each block listed in the blockPointer fields

  // Step 3: mark inode as free
  // Set the "used" field to 0.

  // Step 4: Write out the inode and free block list to disk
  // Move the file pointer to the start of the file
  // Write out the 128 byte free block list
  // Move the file pointer to the position on disk where this inode was stored
  // Write out the inode
}

// List names of all files on disk
void
fs_ls (struct fs_t *fs)
{
  // Step 1: read in each inode and print!
  // Move file pointer to the position of the 1st inode (129th byte)
  // for each inode:
  //   read it in
  //   if the inode is in-use
  //     print the "name" and "size" fields from the inode
  // end for
}

// read this block from this file
void
fs_read (struct fs_t *fs, char name[16], int blockNum, char buf[1024])
{
  // Step 1: locate the inode for this file
  //   - Move file pointer to the position of the 1st inode (129th byte)
  //   - Read in a inode
  //   - If the inode is in use, compare the "name" field with the above file
  //   - If the file names don't match, repeat

  // Step 2: Read in the specified block
  // Check that blockNum < inode.size, else flag an error
  // Get the disk address of the specified block
  // move the file pointer to the block location
  // Read in the block! => Read in 1024 bytes from this location into the buffer
  // "buf"
}

// write this block to this file
void
fs_write (struct fs_t *fs, char name[16], int blockNum, char buf[1024])
{

  // Step 1: locate the inode for this file
  // Move file pointer to the position of the 1st inode (129th byte)
  // Read in a inode
  // If the inode is in use, compare the "name" field with the above file
  // If the file names don't match, repeat

  // Step 2: Write to the specified block
  // Check that blockNum < inode.size, else flag an error
  // Get the disk address of the specified block
  // move the file pointer to the block location
  // Write the block! => Write 1024 bytes from the buffer "buf" to this location
}

// REPL entry point
void
fs_repl ()
{
	/* First line: get name of disk */
	char diskName[16];
	fgets(diskName, sizeof(diskName), stdin);
	strtok(diskName, "\n");

	
}
