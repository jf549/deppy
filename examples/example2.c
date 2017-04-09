#include <stdio.h>

int main(void) {
  printf("FUNCTION START\n");
  int x = 3;
  char y = 2;
  x = x + 1;
  y = y + 4;
  printf("LOOP ENTRY 1\n");
  for (int i = 0; i < 3; ++i) {
    printf("LOOP BODY 1\n");
    y += 1;
    printf("LOOP ENTRY 2\n");
    for (int j = 0; j < 2; ++j) {
      printf("LOOP BODY 2\n");
      x += 1;
    }
  }
  printf("FUNCTION END\n");
  return 0;
}
