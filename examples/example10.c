// An example demonstrating address granularity.
#include <stdlib.h>
#include <stdio.h>

int main(void) {
  int *ip = malloc(sizeof(int));
  char *cp = ((char *) ip) + 1;
  int i;
  char a[2];

  for (i = 0; i < 1; ++i) {
    *ip = 2;
    *cp = 3; // Dependence correctly reported due to granularity.
  }

  for (i = 0; i < 2; ++i) {
    a[i] = 3; // False dependence reported due to granularity.
  }

  return 0;
}
