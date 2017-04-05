#include <analyser/Stride.h>
#include <analyser/StrideDetector.h>

#include <catch.hpp>

TEST_CASE("Number of stride-stride dependences is computed", "[stridedep]") {
  analyser::Stride s0{ 20, 2, 32, true, 1, 1 };
  analyser::Stride s1{ 21, 3, 39, true, 1, 1 };
  REQUIRE(s0.numDependences(s1) == 2);
  REQUIRE(s1.numDependences(s0) == s0.numDependences(s1));
}

TEST_CASE("Strides are detected", "[stridedetect]") {
  analyser::StrideDetector sd;
  REQUIRE(sd.addAddress(10) == false);
  REQUIRE(sd.addAddress(14) == false);
  REQUIRE(sd.addAddress(18) == true);
  REQUIRE(sd.addAddress(14) == true);
  REQUIRE(sd.addAddress(18) == true);
  REQUIRE(sd.addAddress(22) == true);
  REQUIRE(sd.addAddress(30) == false);
  REQUIRE(sd.addAddress(30) == false);
  REQUIRE(sd.addAddress(26) == true);
  REQUIRE(sd.addAddress(30) == true);

  REQUIRE(sd.addAddress(50) == false);
  REQUIRE(sd.addAddress(51) == false);
  REQUIRE(sd.addAddress(52) == false);
  REQUIRE(sd.addAddress(53) == false);
  REQUIRE(sd.addAddress(54) == false);
  REQUIRE(sd.addAddress(55) == true);
}
