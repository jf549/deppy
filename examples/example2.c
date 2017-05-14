/// An example of a loop with two strides and loop-carried dependences between them.
int main(void) {
  char a[30];
  char x = 0;

  for (int i = 0; i < 30; ++i) {
    a[i] = i;
  }

  for (int n = 0; n <= 6; ++n) {
    a[2*n + 10] = 1; // Stride 1 (Write)
    x = a[3*n + 11]; // Stride 2 (Read)
  }

  return 0;
}
