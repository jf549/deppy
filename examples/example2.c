// An example with two strides and loop-carried dependences between them.
int main(void) {
  int x, i, a[30];

  for (i = 0; i < 7; ++i) {
    a[2*i + 10] = 1; // Stride 1 (Write)
    x = a[3*i + 11]; // Stride 2 (Read)
  }

  return 0;
}
