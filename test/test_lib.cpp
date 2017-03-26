#include <lib/gcd.h>

#include <catch.hpp>

TEST_CASE("EGCD is computed", "[egcd]") {
  auto res = lib::egcd(240, 46);
  REQUIRE(res.gcd == 2);
  REQUIRE(res.x == -9);
  REQUIRE(res.y == 47);
}
