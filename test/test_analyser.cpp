#include <analyser/Stride.h>
#include <analyser/StrideDetector.h>

#include <catch.hpp>

TEST_CASE("Number of stride-stride dependences is computed", "[stridedep]") {
  analyser::Stride s0{ 20, 2, 32, true, 1, 1 };
  analyser::Stride s1{ 21, 3, 39, true, 1, 1 };
  analyser::Stride s2{ 13, 4, 53, true, 1, 1 };
  analyser::Stride s3{ 99, 1, 99, true, 1, 1 };
  analyser::Stride s4{ 33, 9999, 33, true, 1, 1 };
  analyser::Stride s5{ 0, 2, 100, true, 1, 1 };
  analyser::Stride s6{ 0, 1, 0, true, 1, 1 };

  REQUIRE(s0.numDependences(s1) == 2);

  REQUIRE(s0.numDependences(s2) == 0);
  REQUIRE(s1.numDependences(s2) == 2);

  REQUIRE(s0.numDependences(s3) == 0);
  REQUIRE(s1.numDependences(s3) == 0);
  REQUIRE(s2.numDependences(s3) == 0);

  REQUIRE(s0.numDependences(s4) == 0);
  REQUIRE(s1.numDependences(s4) == 1);
  REQUIRE(s2.numDependences(s4) == 1);
  REQUIRE(s3.numDependences(s4) == 0);

  REQUIRE(s0.numDependences(s5) == 7);
  REQUIRE(s1.numDependences(s5) == 3);
  REQUIRE(s2.numDependences(s5) == 0);
  REQUIRE(s3.numDependences(s5) == 0);
  REQUIRE(s4.numDependences(s5) == 0);

  REQUIRE(s0.numDependences(s6) == 0);
  REQUIRE(s1.numDependences(s6) == 0);
  REQUIRE(s2.numDependences(s6) == 0);
  REQUIRE(s3.numDependences(s6) == 0);
  REQUIRE(s4.numDependences(s6) == 0);
  REQUIRE(s5.numDependences(s6) == 1);

  REQUIRE(s0.numDependences(s0) == 7);
  REQUIRE(s1.numDependences(s1) == 7);
  REQUIRE(s2.numDependences(s2) == 11);
  REQUIRE(s3.numDependences(s3) == 1);
  REQUIRE(s4.numDependences(s4) == 1);
  REQUIRE(s5.numDependences(s5) == 51);
  REQUIRE(s6.numDependences(s6) == 1);

  REQUIRE(s1.numDependences(s0) == s0.numDependences(s1));
  REQUIRE(s2.numDependences(s0) == s0.numDependences(s2));
  REQUIRE(s2.numDependences(s1) == s1.numDependences(s2));
}

TEST_CASE("Stride-point dependences are computed", "[stridepointdep]") {
  analyser::Stride s0{ 0, 2, 100, true, 1, 1 };

  for (int i = 0; i <= 100; i += 2) {
    REQUIRE(s0.isDependent(i) == true);
    REQUIRE(s0.isDependent(i + 1) == false);
  }

  for (int i = 101; i < 1000; ++i) {
    REQUIRE(s0.isDependent(i) == false);
  }

}

TEST_CASE("Strides are detected", "[stridedetect]") {
  analyser::StrideDetector sd;

  // First stride
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

  // Second stride
  REQUIRE(sd.addAddress(50) == false);
  REQUIRE(sd.addAddress(51) == false);
  REQUIRE(sd.addAddress(52) == false);
  REQUIRE(sd.addAddress(53) == false);
  REQUIRE(sd.addAddress(54) == true);
  REQUIRE(sd.addAddress(55) == true);
  REQUIRE(sd.addAddress(56) == true);
  REQUIRE(sd.addAddress(57) == true);

  // Third stride, fixed address
  REQUIRE(sd.addAddress(22) == false);
  REQUIRE(sd.addAddress(22) == false);
  REQUIRE(sd.addAddress(22) == false);
  REQUIRE(sd.addAddress(22) == false);
  REQUIRE(sd.addAddress(22) == false);
  REQUIRE(sd.addAddress(22) == false);

  // Fourth stride, decreasing
  REQUIRE(sd.addAddress(30) == false);
  REQUIRE(sd.addAddress(27) == false);
  REQUIRE(sd.addAddress(24) == true);
  REQUIRE(sd.addAddress(21) == true);
  REQUIRE(sd.addAddress(18) == true);
  REQUIRE(sd.addAddress(15) == true);
  REQUIRE(sd.addAddress(12) == true);
  REQUIRE(sd.addAddress(9) == true);
  REQUIRE(sd.addAddress(0) == false);
  REQUIRE(sd.addAddress(0) == false);
  REQUIRE(sd.addAddress(6) == true);
  REQUIRE(sd.addAddress(0) == false);
  REQUIRE(sd.addAddress(0) == false);
  REQUIRE(sd.addAddress(3) == false);
}
