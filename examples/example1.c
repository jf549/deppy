// An example demonstrating a fixed-address, loop-carried WAW.
int main(void) {
  int i;
  char c;

  for (i = 0; i < 10; ++i) {
    c = 'a';
  }

  return 0;
}
