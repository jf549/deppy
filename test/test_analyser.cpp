#include <analyser/Stride.h>

#include <catch.hpp>

TEST_CASE("Number of stride-stride dependences is computed", "[stridedep]") {
  analyser::Stride s0{ 20, 2, 32, true, 1, 1};
  analyser::Stride s1{ 21, 3, 39, true, 1, 1};
  REQUIRE(s0.numDependences(s1) == 2);
  REQUIRE(s1.numDependences(s0) == s0.numDependences(s1));
}
