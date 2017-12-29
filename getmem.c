/*
 * Yiyuan Li, Kalkidan Zeberega
 * CSE 374
 * Homework 6
 * 
 * This file contains an implementation for getmem.
 *
 */

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdint.h>

#include "mem.h"
#include "mem_impl.h"

struct free_node* free_list = NULL;
uintptr_t total_size = 0;

void* getBlock(uintptr_t size);
struct free_node* checkFreelist(uintptr_t size);
struct free_node* allocateNewBlock(uintptr_t size);
struct free_node* convertBlockToFreenode
    (void* block_ptr, uintptr_t size);
struct free_node* splitBlock(struct free_node* free_ptr,
                             uintptr_t full_size,
                             uintptr_t get_size);
void addBlockToFreelist(struct free_node* add_ptr);
void* getmem(uintptr_t size);

/*
 * Summary: This function returns a pointer to a new block of storage
 *            with at least size bytes of memory. If getmem cannot 
 *            satisfy the request, then return NULL.
 * Variables: size is the number of bytes of memory 
 *              that has been requested.
 *
 * */
void* getmem(uintptr_t size) {
  // Check if size is greater than or equal to 48.
  if (size < MIN_BLOCK_SIZE) {
    // fprintf(stderr, "Error: Size is too small.\n");
    return NULL;
  }

  // Check if size is a multiple of 16.
  if (size % BLOCK_SIZE_MULT != 0) {
    // fprintf(stderr, "Error: Size is not a multiple of 16.\n");
    return NULL;
  }

  // Do a check_heap at the beginning and end.
  check_heap();

  // Get the block.
  void* block = getBlock(size);

  check_heap();
  total_size += size;
  return block;
}

/*
 * Summary: This function attempts to get a suitable block
 *            to use for the client. It will return a pointer
 *            to the block, or NULL if nothing exists.
 * Variables: size is the number of bytes of memory that 
 *              has been requested.
 *
 * */
void* getBlock(uintptr_t size) {
  // Look through free list to see if a suitable block exists.
  struct free_node* free_result = checkFreelist(size);

  // Check if a block was returned. If not, get a new block.
  if (free_result == NULL) {
    free_result = allocateNewBlock(size);
    if (free_result == NULL) {
      return NULL;
    }
  } else {
    // fprintf(stdout, "A block was found.\n");
  }

  // Check if the block needs to be split up.
  intptr_t dif = free_result->size - SPLIT_THRESHOLD;
  if (dif > 0 && size < dif) {
    splitBlock(free_result, free_result->size, size);
  }

  // Get the void pointer.
  uintptr_t offset = sizeof(struct free_node);
  void* void_ptr = getPointerOffset((void*) free_result, offset);

  return void_ptr;
}

/*
 * Summary: This function checks if there is a suitable block 
 *            to use on the free list. If a block exists, then
 *            it will detatch the block from the free list. It 
 *            will return a pointer to the block, or NULL if
 *            nothing exists.
 * Variables: size is the number of bytes of memory that 
 *              has been requested.
 *
 * */
// Note: When using single pointers, need to assign to ___->next.
//       When using double pointers, need to assign to *___.
struct free_node* checkFreelist(uintptr_t size) {
  struct free_node** current = &free_list;

  // Loop through the linked list to see whats available.
  while (*current != NULL) {
    if ((*current)->size > size) {
      break;
    } else {
      current = &(*current)->next;
    }
  }

  // Detach from the tree.
  if (*current != NULL) {
    struct free_node* temp = *current;
    *current = (*current)->next;
    temp->next = NULL;
    return temp;
  } else {
    return NULL;
  }
}

/*
 * Summary: This function gets a void* block of memory from the
 *            underlying system. It will return the size of block
 *            that got allocated.
 * Variables: size is the number of bytes of memory that has 
 *              been requested.
 *            free_ptr is the pointer to assign the block to.
 *
 * */
struct free_node* allocateNewBlock(uintptr_t size) {
  uintptr_t block_size = 0;
  if (size < 8000) {
    block_size = 8000;
  } else {
    block_size = size;
  }

  // Attempt to get a suitable block.
  void* block_ptr = malloc(sizeof(struct free_node) + block_size);

  // Check if the malloc request was valid.
  if (block_ptr == NULL) {
    // fprintf(stderr, "Error: The malloc request failed.\n");
    return 0;
  }

  // Need to convert part of ptr to a free_node.
  struct free_node* free_ptr = convertBlockToFreenode(block_ptr, block_size);
  return free_ptr;
}

/*
 * Summary: This function takes a void* block of memory and converts
 *            the top portion into a free_node object. It leaves the
 *            rest of the block as a void*.
 * Variables: free_ptr is the pointer to assign the free node to.
 *            block_ptr is the pointer to the beginning of the block.
 *
 * */
struct free_node* convertBlockToFreenode
            (void* block_ptr, uintptr_t size) {
  // Check if block is NULL.
  if (block_ptr == NULL) {
    // fprintf(stderr, "Error: block_ptr is NULL.\n");
    return NULL;
  }


  // Convert top of block to a free_node.
  struct free_node* free_ptr = (struct free_node*) block_ptr;
  free_ptr->size = size;
  free_ptr->next = NULL;

  return free_ptr;
}

/*
 * Summary: This function splits up the block thats provided into
 *            two separate blocks of memory. It will add the bottom
 *            block (greater address) to the free list and assign
 *            the top block to the outside pointer.
 *          Note: Maybe should be done during checking the free list.
 * 
 * Variables: free_ptr is the pointer to the free node that needs to
 *              be split up.
 *            full_size is the size of free_ptr's empty block.
 *            get_size is the requested size from the client.
 * */
struct free_node* splitBlock(struct free_node* free_ptr,
                             uintptr_t full_size,
                             uintptr_t get_size) {
  // Check if before_node is NULL.
  if (free_ptr == NULL) {
    // fprintf(stderr, "Error: block is NULL.");
    return NULL;
  }

  // Get the pointer to the beginning of the second free node.
  uintptr_t offset = sizeof(struct free_node) + get_size;
  struct free_node* free_temp_ptr =
      (struct free_node*) getPointerOffset((void*) free_ptr, offset);

  // Readjust the two blocks of memory.
  free_ptr->size = get_size;
  free_ptr->next = NULL;
  free_temp_ptr->size = full_size - (get_size + sizeof(struct free_node));
  free_temp_ptr->next = NULL;

  // Add the new block back to free list.
  addBlockToFreelist(free_temp_ptr);

  // Return the free node that is set for allocation.
  return free_ptr;
}

/*
 * Summary: This function adds a individual free node to
 *            the free list in the appropriate spot.
 * Variables: free_ptr is the pointer to the free node that needs to
 *              be placed in the free list.
 *
 * */
void addBlockToFreelist(struct free_node* free_ptr) {
  struct free_node** current = &free_list;

  // Loop over the linkedlist.
  while (*current != NULL) {
    // Check if current.next has a greater address than free_ptr.
    int compare_ptr = comparePtrs((void*) *current, (void*) free_ptr);
    if (compare_ptr == 1) {
      break;
    }

    current = &(*current)->next;
  }

  // Insert the free node into the proper spot.
  free_ptr->next = *current;
  *current = free_ptr;
}
