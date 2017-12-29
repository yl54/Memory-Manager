/*
 * Yiyuan Li, Kalkidan Zeberega
 * CSE 374
 * Homework 6
 * 
 * This file contains an implementation for print_heap.
 *
 */

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>

#include "mem.h"
#include "mem_impl.h"

/*
 * Summary: This function prints a formatted listing on file f showing the
 *            blocks on the free list. Each line of output describes one
 *            free block.
 * Variables: f is the file to print the block information to.
 *
 * */
void print_heap(FILE* f) {
  // Open the file, overwrite the previous stuff in it.
  if (f == NULL) {
    // fprintf(stderr, "Error: File failed to open.");
    return;
  }

  // Get a pointer to the beginning of the free list.
  struct free_node *free_ptr;
  free_ptr = (struct free_node*) free_list;

  // For each block on the free list, print a line of output
  //   that describes the free block, its address, and
  //   its length.
  fprintf(f, "Start of free list.\n");
  fprintf(f, "\n------------------\n");
  while (free_ptr != NULL) {
    fprintf(f, "Block address: %p | ", free_ptr);
    fprintf(f, "Block size: %" PRIuPTR ".\n", free_ptr->size);
    free_ptr = free_ptr->next;
  }

  fprintf(f, "------------------\n");
  fprintf(f, "\nEnd of free list.\n");
  fclose(f);
}



