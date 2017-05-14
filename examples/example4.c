#include "extra.h"

int main(void) {
  int x = 0, y = 0;

  for (int i = 0; i < 5; ++i) {
    for (int j = 0; j < 3; ++j) {
      x = f(y + i + j);
    }
  }
}
