// An example demonstrating a fixed-address, loop-carried WAW.
int main(void) {
  int i, c;

  for (i = 0; i < 10; ++i) {
    c = 3;
  }

  return 0;
}
