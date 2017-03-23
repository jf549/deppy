#include "gcd.h"

#include <iostream>

namespace lib {
  // Implemented from CLRS3
  template<typename T>
  EgcdResult<T> egcd(T a, T b) {
    T x0 = 1, x1 = 0, y0 = 0, y1 = 1, q, r, xn, yn;

    while (b != 0) {
      q = a / b;
      r = a % b;
      a = b;
      b = r;

      xn = x0 - q * x1;
      x0 = x1;
      x1 = xn;

      yn = y0 - q * y1;
      y0 = y1;
      y1 = yn;
    }

    return {a, x0, y0};
  }
}

int main() {
  auto res = lib::egcd(240, 46);
  std::cout << res.gcd << ", " << res.x << ", " << res.y << std::endl;
}
