#include <analyser/Stride.h>
#include <analyser/StrideDetector.h>
#include <analyser/StrideLoop.h>

#include <catch.hpp>

TEST_CASE("Number of stride-stride dependences is computed", "[stridedep]") {
  analyser::Stride s0{ 20, 2, 32, 1, true };
  analyser::Stride s1{ 21, 3, 39, 1, true };
  analyser::Stride s2{ 13, 4, 53, 1, true };
  analyser::Stride s3{ 99, 1, 99, 1, true };
  analyser::Stride s4{ 33, 9999, 33, 1, true };
  analyser::Stride s5{ 0, 2, 100, 1, true };
  analyser::Stride s6{ 0, 1, 0, 1, true };

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
  analyser::Stride s0{ 0, 2, 100, 1, true };

  for (unsigned i = 0; i <= 100; i += 2) {
    REQUIRE(s0.isDependent(i) == true);
    REQUIRE(s0.isDependent(i + 1) == false);
  }

  for (unsigned i = 101; i < 1000; ++i) {
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

TEST_CASE("Strides distances are computed", "[stridedist]") {
  analyser::StrideDetector sd;

  // First stride
  sd.addAddress(10);
  sd.addAddress(14);
  sd.addAddress(18);
  sd.addAddress(14);
  sd.addAddress(18);
  sd.addAddress(22);

  REQUIRE(sd.getStride() == 4);

  // Second stride, fixed address
  sd.addAddress(50);
  sd.addAddress(50);
  sd.addAddress(50);
  sd.addAddress(50);
  sd.addAddress(50);
  sd.addAddress(50);

  REQUIRE(sd.getStride() == 0);

  // Third stride, decreasing
  sd.addAddress(30);
  sd.addAddress(27);
  sd.addAddress(24);
  sd.addAddress(21);
  sd.addAddress(18);
  sd.addAddress(15);
  sd.addAddress(12);

  REQUIRE(sd.getStride() == 3);
}

TEST_CASE("Strides are merged", "[stridemerge]") {
  analyser::Stride s0{ 20, 2, 32, 0, true };
  analyser::Stride s1{ 21, 3, 39, 1, true };
  analyser::Stride s2{ 34, 2, 98, 2, true };

  REQUIRE(s0.merge(s1) == false);
  REQUIRE(s1.merge(s2) == false);

  REQUIRE(s0.merge(s2) == true);

  REQUIRE(s0.base == 20);
  REQUIRE(s0.limit == 98);
  REQUIRE(s0.iterLastAccessed == 2);
}
