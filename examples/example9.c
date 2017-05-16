// An example where address aliasing causes a false dependence.
#include <stdlib.h>

int main(void) {
  int i, *p;

  for (i = 0; i < 1000; ++i) {
    p = malloc(sizeof(int));
    *p = 3;
    free(p);
  }

  return 0;
}
