int main(void) {
  int x = 3;
  char y = 2;
  x = x + 1;
  y = y + 4;

  for (int i = 0; i < 6; ++i) {
    y = y + 1;
    for (int j = 0; j < 6; ++j) {
      x = x + 1;
    }
  }

  return 0;
}
