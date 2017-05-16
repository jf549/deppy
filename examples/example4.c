// An example with nested stride accesses.
int main(void) {
  int i, j, x, a[30] = { 0 };

  for (i = 0; i < 8; ++i) {
    a[2*i + 10] = 3;
    for (j = 0; j < 7; ++j) {
      x = a[3*j + 11];
    }
  }

  return 0;
}
