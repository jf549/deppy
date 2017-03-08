int main(void) {
  int x = 3;
  char y = 2;
  x = x + 1;
  y = y + 4;
  x = x + 2;
  for (int i = 0; i < 5; ++i) {
    y += 1;
    for (int j = 0; j < 2; ++j) {
      x += 1;
      for (int k = 0; k < 2; ++k) {
        x += 2;
      }
    }
  }
  return 0;
}
