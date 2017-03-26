#include <lib/gcd.h>

#include <iostream>

int main() {
  auto res = lib::egcd(240, 46);
  std::cout << res.gcd << ", " << res.x << ", " << res.y << std::endl;
}
