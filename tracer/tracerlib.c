#include <stdio.h>

void print(int isLoad, void *addr) {
  printf("%s: %p\n", isLoad ? "Load" : "Store", addr);
}
