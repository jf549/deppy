// An example with a dependence on a global, across a function call.
static int x = 10;

int f(void) {
  return x + 3;
}

int main(void) {
  int i;

  for (i = 0; i < 10; ++i) {
    x = f();
  }

  return 0;
}
