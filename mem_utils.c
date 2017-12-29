/*
 * Yiyuan Li, Kalkidan Zeberega
 * CSE 374
 * Homework 6
 * 
 * This file contains an implementation of function check_heap.
 * This file also stores shared code or functions so that they are 
 *   used internally by other parts of the implementation, but they 
 *   are not intended to be part of the public interface. 
 *
 */

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdint.h>
#include<assert.h>

#include "mem_impl.h"

void check_heap();
void* getPointerOffset(void* ptr, uintptr_t offset);
int comparePtrs(void* ptr_1, void* ptr_2);

/*
 * Summary: This function checks for possible problems with the free list.
 *            It verifies that several conditions are either being met
 *            or not. This function will check errors in functions such 
 *            as getmem and freemem at the beginning and end. It will
 *            return silenty if there are no errors, if there is an error,
 *            the program will terminate at that point.
 * Variables: N/A.
 *
 * */
void check_heap() {
  struct free_node* current = free_list;
  while (current != NULL) {
    // Check if block sizes are positive and
    //   no smaller than the minimum size.
    assert(current->size >= MIN_BLOCK_SIZE);
    if (current->next != NULL) {
      // Check if blocks are ordered with strictly
      //   increasing memory addresses.
      int isIncreasing =
            comparePtrs((void*) current, (void*) current->next);
      assert(isIncreasing == -1);

      // Check if there is overlapping between blocks.
      uintptr_t offset = sizeof(struct free_node) + current->size;
      struct free_node* current_end =
          (struct free_node*) getPointerOffset((void*) current, offset);
      int isNotOverlapping =
          comparePtrs((void*) current_end, (void*) current->next);
      assert(isNotOverlapping == -1);
    }
    current = current->next;
  }
}

// Function to perform pointer arithmetic and get the
//   pointer from that offset.
void* getPointerOffset(void* ptr, uintptr_t offset) {
  if (ptr == NULL) {
    // fprintf(stderr, "Error: ptr is null.");
    return ptr;
  }

  // Get the initial address of ptr.
  uintptr_t adr = (uintptr_t) ptr;

  // Add or subtract the offset from the initial address.
  adr += offset;

  // Return a pointer to that spot.
  return (void*) adr;
}

// Function to compare pointer addresses. Return -1 if
//   ptr_1 is before ptr_2, 0 if same, and 1 if ptr_1
//   is after ptr_2. Return 2 if something weird happens.
int comparePtrs(void* ptr_1, void* ptr_2) {
  uintptr_t val_ptr_1 = (uintptr_t) ptr_1;
  uintptr_t val_ptr_2 = (uintptr_t) ptr_2;

  if (val_ptr_1 < val_ptr_2) {
    return -1;
  } else if (val_ptr_1 == val_ptr_2) {
    return 0;
  } else if (val_ptr_1 > val_ptr_2) {
    return 1;
  }

  return 2;
}
