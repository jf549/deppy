#include <lib/BoundedBuffer.hpp>
#include <lib/event.h>

#include <catch.hpp>

#include <array>
#include <future>
#include <vector>

TEST_CASE("Bounded buffer can be accessed sequentially", "[boundedbuf]") {
  lib::BoundedBuffer<event_t> buf;
  std::vector<event_t> a{ { LOAD, LOAD, STORE, LOOP_ENTRY, LOOP_ITER, LOOP_EXIT } };
  std::vector<event_t> res;

  for (auto item : a) {
    buf.produce(item);
  }

  buf.produce(SENTINAL);

  event_t item;

  while ((item = buf.consume()) != SENTINAL) {
    res.push_back(item);
  }

  REQUIRE(a == res);
}

TEST_CASE("Bounded buffer can be accessed concurrently", "[boundedbuf]") {
  lib::BoundedBuffer<int> buf(3);
  std::array<int, 10> a{ { -1, -2, -3, 38, 21, 0, 0, 0, INT_MAX, INT_MIN } };

  auto futProd = std::async([&buf, &a]{
    for (auto item : a) {
      buf.produce(item);
    }
  });

  auto futCon = std::async([&buf]{
    std::array<int, 10> res;
    for (unsigned i = 0; i < 10; ++i) {
      res[i] = buf.consume();
    }
    return res;
  });

  futProd.get();

  REQUIRE(futCon.get() == a);
}
