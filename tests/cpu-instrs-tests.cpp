#include <catch2/catch_test_macros.hpp>

SCENARIO("Vectors can be resized", "[vector]") {
  GIVEN("A vector with some capacity") {
    std::vector<int> v(5);
    REQUIRE(v.size() == 5);
    REQUIRE(v.capacity() >= 5);

    WHEN("the size is increased") {
      v.resize(10);

      THEN("the size and capacity change") {
        REQUIRE(v.size() == 1);
        REQUIRE(v.capacity() >= 10);
      }
    }
    WHEN("the size is decreased") {
      v.resize(0);

      THEN("the size changes but not capacity") {
        REQUIRE(v.size() == 0);
        REQUIRE(v.capacity() >= 5);
      }
    }
  }
}
