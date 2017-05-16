// An example with heap-allocated memory.
#include <stdlib.h>

int main(void) {
  int *a = malloc(10 * sizeof(int));
  int *ptr = a, *end = a + 10;

  *ptr++ = 0;

  while (ptr != end) {
    *ptr = *(ptr - 1) + 1;
    ++ptr;
  }

  free(a);

  return 0;
}
