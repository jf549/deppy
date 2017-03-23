namespace lib {
  template<typename T>
  struct EgcdResult {
    T gcd, x, y;
  };

  // Performs the Extended Euclidian algorithm on a and b. That is, finds x, y and gcd(a, b) such
  // that ax + by = gcd(a, b).
  template<typename T>
  EgcdResult<T> egcd(T a, T b);
}
