/*
 * Yiyuan Li, Kalkidan Zeberega
 * CSE 374
 * Homework 6
 * 
 * This file contains an implementation for freemem.
 *
 */

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#include "mem.h"
#include "mem_impl.h"

void freemem(void* p);
void returnToFreelist(struct free_node* free_ptr);
void checkIfGlueBlocks(struct free_node* free_ptr,
                       struct free_node* before_free_ptr);
int checkIfAdjacent(struct free_node* first_block,
                    struct free_node* second_block);
void glueBlocks(struct free_node* first_block,
                struct free_node* second_block);

/*
 * Summary: This function returns the block of storage at location
 *            p to the pool of available free storage. p must be
 *            obtained from getmem.
 * Variables: p is a pointer that holds a block of memory.
 *
 * */
// For now, assume that a free node will always be above.
void freemem(void* p) {
  // Check if p is NULL.
  if (p == NULL) {
    // fprintf(stderr, "Error: Provided pointer is NULL.\n");
    return;
  }

  // Subtract a few bytes to get the front of the free node.
  uintptr_t offset_to_free_node = -sizeof(struct free_node);
  struct free_node* free_ptr =
       (struct free_node*) getPointerOffset(p, offset_to_free_node);

  // Return the block of memory to the free list.
  check_heap();
  returnToFreelist(free_ptr);
  check_heap();
}

// Function to put p on the free list.
void returnToFreelist(struct free_node* free_ptr) {
  // Check if free_ptr is NULL.
  if (free_ptr == NULL) {
    // fprintf(stderr, "Error: Provided pointer is NULL.\n");
    return;
  }

  // Get the head of the free list.
  struct free_node** current = &free_list;

  // Keep track of ptr before, initialize it as NULL.
  struct free_node* before = NULL;

  // Loop through the free list.
  while (*current != NULL) {
    int compare_ptrs = comparePtrs((void*) *current, (void*) free_ptr);
    if (compare_ptrs == 1) {
      break;
    } else if (compare_ptrs == 0) {
      return;
    }

    before = (*current);
    current = &(*current)->next;
  }

  free_ptr->next = *current;
  *current = free_ptr;

  checkIfGlueBlocks(free_ptr, before);
}

// Function to check if the current free node is
//   adjacent to previous and next free block.
// Assume that free_ptr exists.
// Need 2 params because before ptr may not exist.
void checkIfGlueBlocks(struct free_node* free_ptr,
                       struct free_node* before_free_ptr) {
  if (before_free_ptr != NULL) {
    if (checkIfAdjacent(before_free_ptr, free_ptr) == 0) {
      glueBlocks(before_free_ptr, free_ptr);
      free_ptr = before_free_ptr;
    }
  }

  // Check if there is a block after free_ptr.
  if (free_ptr->next != NULL) {
    if (checkIfAdjacent(free_ptr, free_ptr->next) == 0) {
      glueBlocks(free_ptr, free_ptr->next);
    }
  }
}

// Function to see if two blocks are adjacent. Returns 0
//   if adjacent, 1 if not adjacent or applicable.
int checkIfAdjacent(struct free_node* first_block,
                    struct free_node* second_block) {
  // Check if either first_block or second_block is NULL.
  if (first_block == NULL || second_block == NULL) {
    fprintf(stderr, "Error: One of the block pointers is NULL.\n");
    return 1;
  }

  // Check if the end of the first address is equal to the
  //   beginning of the second_block.
  uintptr_t offset_end_adr = sizeof(struct free_node) + first_block->size;
  struct free_node* end_of_first_block =
         (struct free_node*)
         getPointerOffset((void*) first_block, offset_end_adr);
  if (end_of_first_block == second_block) {
    return 0;
  } else {
    return 1;
  }
}

// Function to glue two blocks.
void glueBlocks(struct free_node* first_block,
                struct free_node* second_block) {
  // Check if either pointer is NULL.
  if (first_block == NULL || second_block == NULL) {
    fprintf(stderr, "Error: One of the block pointers is NULL.\n");
    return;
  }

  uintptr_t glue_size = first_block->size +
          sizeof(struct free_node) + second_block->size;
  first_block->size = glue_size;
  first_block->next = second_block->next;

  second_block->next = NULL;
}

