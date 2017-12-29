/*
 * NOTE: THIS FILE IS NOT PART OF THE STUFF THAT
 *         WE TURN IN FOR THIS PROJECT. 
 *
 * This file contains a quick program that will help
 *   for debugging purposes. This will have situations
 *   where we should exactly know what will happen, making
 *   bugs/wierd behavior easy to spot. 
 *
 */

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <inttypes.h>

#include "mem.h"
#include "mem_impl.h"

void testVoidBlocks();
void testSingleBlock();
void* getVoidBlock();
void addToPointer();
void testConvertFreenode();
void* getVoidBlock();
void convertVoidToFreenode (struct free_node* f_node, void* block);
int checkNullGetFree();
int checkSingleGetFree();
void printStats(double duration);
int checkMultipleGetFree();
void checkModTest();
void checkUintptrTest();

int main() {
  // testVoidBlocks();
  // testConvertFreenode();
  // checkSingleGetFree();
  // checkModTest();
  checkUintptrTest(); 
  return EXIT_SUCCESS;
}

void testVoidBlocks() {
  //testSingleBlock();
  //testSingleBlock();
  addToPointer();
}

// void blocks have multiplier of 1.
// uintptr_t is an unsigned integer that can hold
//   a pointer address value.
void testSingleBlock() {
  int* block = getVoidBlock();
  uintptr_t ptr = &block;
  // Get addresses.
  uintptr_t ptr_2 = ptr + 1;
  // Print the addresses.
  fprintf(stdout, "ptr_1: %d.\n", ptr);
  fprintf(stdout, "ptr_2: %d.\n", ptr_2);
  free(block);
}

// Q: How big is a uintptr_t?
// A: Seems to be 8.
// Q: How big is a void?
// A: Seems to be 1.
void addToPointer() {
  void* block = getVoidBlock();
  fprintf(stdout, "block: %d.\n", block);
  for (int i = -4; i < 4; i++) {
    fprintf(stdout, "block + %d = %d\n", i, block + i);
  }
}
/*
Lesson: Multipliers are dependent on the cast thats on the operation.
Sample output:
block_ptr:   6311952.
block_ptr_2: 6311968. void to void
free_ptr:    6312208. void to free_node
free_ptr_2:  6312464. free_node to free_node
block_ptr_3: 6312480. free_node to void

*/
void testConvertFreenode() {
  struct free_node* free_ptr = NULL;
  void* block_ptr = getVoidBlock();
  void* block_ptr_2 = (void*) block_ptr + 16;
  free_ptr = (struct free_node*) block_ptr + 16;
  struct free_node* free_ptr_2 = free_ptr + 16;
  void* block_ptr_3 = (void*) free_ptr_2 + 16;
  free_ptr->size = (uintptr_t) 678;

  fprintf(stdout, "size: %d.\n", free_ptr->size);
  fprintf(stdout, "block_ptr: %d.\n", block_ptr);
  fprintf(stdout, "block_ptr_2: %d.\n", block_ptr_2);
  fprintf(stdout, "free_ptr: %d.\n", free_ptr);
  fprintf(stdout, "free_ptr_2: %d.\n", free_ptr_2);
  fprintf(stdout, "block_ptr_3: %d.\n", block_ptr_3);
  fprintf(stdout, "free node size: %d.\n", sizeof(struct free_node));
  fprintf(stdout, "void size: %d.\n", sizeof(void*));
}


// Function to convert a void* block to a free_node block.
void convertVoidToFreenode (struct free_node* f_node, void* block) {
  // Get a void pointer to a void block thats 16 below.
  void* ptr_2 = block + 16;
  f_node = (struct free_node*) block;
  // Need to have a free node malloc function.
  
  // Cast the very top of the node to free_node pointer.
  block = ptr_2;
}

// This function will get a NULL pointer and attempt
//   to free a NULL block of memory. Returns 0 if successful.
int checkNullGetFree() {
  return 0;
}

// This function will get one block of memory, check
//   its properties, and free it. Returns 0 if successful.
int checkSingleGetFree() {
  time_t init_time;
  time(&init_time);
  
  // Initialize the array.
  void* results[100];
  // These requests should get the initial blocks and subract
  //   from that particular block.
  results[0] = getmem(1600);
  print_heap();
  results[1] = getmem(160);
  print_heap();
  results[2] = getmem(1600);
  print_heap();
  // These requests should not add blocks to the back of the free list.
  results[3] = getmem(160000);
  results[4] = getmem(200000);
  
  
  results[5] = getmem(3008);
  print_heap();
  results[6] = getmem(3008);
  print_heap();
  results[7] = getmem(3008);
  print_heap();
  results[8] = getmem(3008);
  

  print_heap();
  freemem(results[3]);
  print_heap();
  freemem(results[4]);
  print_heap();
  results[3] = getmem(180000);
  print_heap();
  freemem(results[0]);
  print_heap();
  freemem(results[1]);
  print_heap();
  
  time_t end_time;
  time(&end_time);
  
  double duration = difftime(end_time, init_time);
  printStats(duration);
  return 0;
}

// Function to print some statistics.
void printStats(double duration) {
  uintptr_t total_size = 0;
  uintptr_t total_free = 0;
  uintptr_t n_free_blocks = 0;
  get_mem_stats(&total_size, &total_free, &n_free_blocks);
  double average_bytes = total_free/n_free_blocks;
  fprintf(stdout, "---------------------------------\n");
  fprintf(stdout, "Total Processor Time: %f.\n",  duration);
  fprintf(stdout, "Total Memory Allocated: %" PRIuPTR ".\n", total_size);
  fprintf(stdout, "Total Memory on Free Blocks: %" PRIuPTR ".\n", total_free);
  fprintf(stdout, "Current Number of Free Blocks: %" PRIuPTR ".\n", n_free_blocks);
  fprintf(stdout, "Average Bytes of Free Blocks: %f.\n", average_bytes);  
  fprintf(stdout, "---------------------------------\n");
}

// This function will get a few blocks of memory, check their
//   properties, and free them. Returns 0 if successful.
int checkMultipleGetFree() {
  return 0;
}

// Function to get a block of memory with a void* return.
void* getVoidBlock() {
  void* block = malloc(1000);
  return block;
}

// Function to check mod capabilities.
void checkModTest() {
  int p = 10 % 16;
  int q = 43 % 16;
  fprintf(stdout, "p: %d.\n", p);
  fprintf(stdout, "q: %d.\n", q);
}

void checkUintptrTest() {
  uintptr_t p = 100;
  uintptr_t w = p - 120;  
  int q = p - 120;
  fprintf(stdout, "p: %d.\n", p);
  fprintf(stdout, "w: %d.\n", w); 
  fprintf(stdout, "q: %d.\n", q);
}

/*
uintptr info:
Integer types capable of holding object pointers

The following type designates a signed integer type with the property that any valid pointer to void can be converted to this type, then converted back to a pointer to void, and the result will compare equal to the original pointer: intptr_t.

The following type designates an unsigned integer type with the property that any valid pointer to void can be converted to this type, then converted back to a pointer to void, and the result will compare equal to the original pointer: uintptr_t.

Maximum value of pointer-holding unsigned integer type:

{UINTPTR_MAX}
    2 16 -1
*/


