#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "noomr.h"

#define NUM_ROUNDS 50000

// Random number generation based off of http://www.azillionmonkeys.com/qed/random.html
#define RS_SCALE (1.0 / (1.0 + RAND_MAX))

size_t random_class () {
    double d;
    do {
       d = (((rand () * RS_SCALE) + rand ()) * RS_SCALE + rand ()) * RS_SCALE;
    } while (d >= 1); /* Round off */
    return d * NUM_CLASSES;
}

size_t uniform_size_class() {
  return ALIGN(CLASS_TO_SIZE(random_class()) - sizeof(block_t));
}

volatile bool spec = false;

void record_allocation(void * p, size_t s) {

}

bool speculating() {
  return spec;
}

int main() {
  int * sequential = noomr_malloc(sizeof(int));
  int * ptrs[NUM_ROUNDS] = {NULL};
  ptrs[0] = sequential;
  beginspec();
  spec = true;


  int rnd, check;
  size_t alloc_size;

  srand(0);

  for (rnd = 1; rnd < NUM_ROUNDS; rnd++) {
    alloc_size = uniform_size_class();
    int * payload = noomr_malloc(alloc_size);
    assert(noomr_usable_space(payload) >= alloc_size);
    ptrs[rnd] = payload;
    for (check = 0; check < rnd; check++) {
      if (ptrs[check] == payload) {
        fprintf(stderr, "Duplicate allocation found indexes %d %d\n", rnd, check);
        exit(-1);
      }
    }
    *payload = 42;
  }
  endspec();
  printf("Basic allocation test passed\n");
  print_noomr_stats();
}