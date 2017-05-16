// An example with a dependence on a local variable through a function call.
#include <stdlib.h>

void f(long *l, int offset) {
  l[offset] = 1;
}

int main(void) {
  long *l = malloc(10 * sizeof(l));
  int i;

  for (i = 0; i < 20; ++i) {
    f(l, i % 10);
  }

  return 0;
}
