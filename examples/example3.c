// An example demonstrating nested loops with fixed-address accesses.
int main(void) {
  int i, j, x = 0, y = 0;

  for (i = 0; i < 10; ++i) {
    y = y + i;
    for (j = 0; j < 10; ++j) {
      x = y + j;
    }
  }

  return 0;
}
