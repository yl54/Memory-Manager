/*
 * Yiyuan Li, Kalkidan Zeberega
 * CSE 374
 * Homework 6
 * 
 * This file contains an implementation for bench.
 *
 */

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include <inttypes.h>

#include "mem.h"

int pickGetOrFree(int pctget);
void* getMemBlock(int pctlarge, int small_limit, int large_limit);
int freeBlock(void* blocks[], int amt);
void printStats(double duration);
int getRandomNumber(int m);

static int random_seed;

int main(int argc, char** argv) {
  int ntrials = 10000;
  int pctget = 50;
  int pctlarge = 10;
  int small_limit = 200;
  int large_limit = 20000;
  random_seed = -1;

  // Check if any optional args exist.
  for (int i = 1; i < argc; i++) {
    if (i == 1) {
      ntrials = atoi(argv[i]);
    }
    if (i == 2) {
      pctget = atoi(argv[i]);
    }
    if (i == 3) {
      pctlarge = atoi(argv[i]);
    }
    if (i == 4) {
      small_limit = atoi(argv[i]);
    }
    if (i == 5) {
      large_limit = atoi(argv[i]);
    }
    if (i == 6) {
      random_seed = atoi(argv[i]);
    }
  }

  // Check if random_seed has been set.
  if (random_seed != -1) {
    srand(random_seed);
  } else {
    srand(time(NULL));
  }

  // Initialize the starting time.
  time_t init_time;
  time_t current_time;
  time(&init_time);
  double duration = 0;

  // Initialize an array of pointers to blocks of memory.
  void* blocks[ntrials];
  for (int i = 0; i < ntrials; i++) {
    blocks[i] = NULL;
  }

  int amt = 0;
  int print_stats_mod = ntrials / 10;
  if (print_stats_mod == 0) {
    print_stats_mod = 1;
  }

  fprintf(stdout, "Bench: Start of tests.\n");
  for (int i = 0; i < ntrials; i++) {
    int status_get_free = pickGetOrFree(pctget);
    if (status_get_free == 0) {
      // Get a memory block.
      void* result = getMemBlock(pctlarge, small_limit, large_limit);
      if (result != NULL) {
        // Initialize first 16 bytes to 0xFE.
        unsigned char* b = (unsigned char*) result;
        *b = 0xFE;
        blocks[amt] = result;
        amt++;
      }
    } else if (status_get_free == 1) {
      // Free a memory block.
      int status = freeBlock(blocks, amt);
      if (status == 0) {
        amt--;
      }
    }

    if (i % print_stats_mod == 0) {
      // Check the time difference.
      time(&current_time);
      duration = difftime(current_time, init_time);

      // Should print stats approx 10 times during full test.
      printStats(duration);
    }
  }

  fprintf(stdout, "Bench: End of tests.\n");
  time_t end_time;
  time(&end_time);
  duration = difftime(end_time, init_time);
  printStats(duration);

  FILE* fp = fopen("free_list_output.txt", "w");
  print_heap(fp);
  return EXIT_SUCCESS;
}

// Function to pick a get or free request. Return 0 to signal
//   a get request and 1 to signal a free request. Return
//   2 if something else happens.
int pickGetOrFree(int pctget) {
  int choice = getRandomNumber(100);
  if (choice < pctget) {
    return 0;
  } else if (choice > pctget) {
    return 1;
  // If something weird happens, return 2.
  } else {
    return 2;
  }
}

// Function to pick a block of memory.
void* getMemBlock(int pctlarge, int small_limit, int large_limit) {
  int large_choice = getRandomNumber(100);
  int size = 0;

  // Check if the number is large.
  if (large_choice <= pctlarge) {
    size = getRandomNumber(large_limit);
    if (size < small_limit) {
      size += (large_limit - small_limit);
    }
  } else if (large_choice > pctlarge && large_choice < 100) {
    size = getRandomNumber(small_limit);
  } else {
    fprintf(stderr, "Error: getMemBlock has a weird result.\n");
  }

  while (size % 16 != 0) {
    size += 1;
  }
  return getmem(size);
}

// Function to pick a random block and free it to the free list.
// Returns 0 if successful, 1 if not.
int freeBlock(void* blocks[], int amt) {
  // Check if there is anything in the block.
  if (amt == 0) {
    return 1;
  }

  int choice = getRandomNumber(amt);
  if (blocks[choice] != NULL) {
    // Free the block at that index.
    freemem(blocks[choice]);

    // Move the block from the last index to the vacated.
    blocks[choice] = blocks[amt - 1];
    blocks[amt - 1] = NULL;
    return 0;
  } else {
    return 1;
  }
}

// Function to print some statistics.
void printStats(double duration) {
  uintptr_t total_size = 0;
  uintptr_t total_free = 0;
  uintptr_t n_free_blocks = 0;
  get_mem_stats(&total_size, &total_free, &n_free_blocks);
  double average_bytes = 0;
  if (n_free_blocks != 0) {
    average_bytes = total_free/n_free_blocks;
  }

  fprintf(stdout, "---------------------------------\n");
  fprintf(stdout, "Total Processor Time: %f.\n",  duration);
  fprintf(stdout, "Total Memory Allocated: %" PRIuPTR ".\n", total_size);
  fprintf(stdout, "Total Memory on Free Blocks: %" PRIuPTR ".\n",
                   total_free);
  fprintf(stdout, "Current Number of Free Blocks: %" PRIuPTR ".\n",
          n_free_blocks);
  fprintf(stdout, "Average Bytes of Free Blocks: %f.\n", average_bytes);
  fprintf(stdout, "---------------------------------\n");
}

// Function to get random number.
int getRandomNumber(int m) {
  if (m == 0) {
    m = 1;
  }

  // Get a random seed and number.
  int r = rand_r(&random_seed);

  // Mod this number by amt.
  int choice = r % m;
  return choice;
}
