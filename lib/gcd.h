namespace lib {
  template<typename T>
  struct EgcdResult {
    T gcd, x, y;
  };

  // Performs the Extended Euclidian algorithm on a and b. That is, finds x, y and gcd(a, b) such
  // that ax + by = gcd(a, b).
  // Implemented from CLRS3.
  template<typename T>
  EgcdResult<T> egcd(T a, T b) {
    T x0 = 1, x1 = 0, y0 = 0, y1 = 1, q, r, xn, yn;

    while (b) {
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

    return { a, x0, y0 };
  }
}
