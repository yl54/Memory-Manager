/*
 * Yiyuan Li, Kalkidan Zeberega
 * CSE 374
 * Homework 6
 * 
 * This is a header file for private functions 
 *   in the memory manager.
 *
 */

#ifndef MEM_IMPL_H
#define MEM_IMPL_H

#include <stdint.h>

// Define a minimum block size.
#define MIN_BLOCK_SIZE 48

// Define a threshold for splitting blocks.
#define SPLIT_THRESHOLD 96

// Define the number the block must be a multiple of.
#define BLOCK_SIZE_MULT 16

// Declare the free list data structure here.
struct free_node {
  uintptr_t size;
  struct free_node* next;
};

extern struct free_node* free_list;
extern uintptr_t total_size;

void check_heap();
void* getPointerOffset(void* ptr, uintptr_t offset);
int comparePtrs(void* ptr_1, void* ptr_2);

// Any other functions in mem_utils.c or shared functions
//   that deal with internal functionality of memory management.

#endif
