#include "interface.hpp"
#include <catch2/catch_template_test_macros.hpp>

TEST_CASE("Optional Comparisons", "[comparisons]")
{
  SECTION("Equality and Inequality of Optionals")
  {
    NonNegOpt<int> a = 4;
    NonNegOpt<int> b(42);

    REQUIRE(!(a == b));
    REQUIRE(a == a);
    REQUIRE(a != b);
    REQUIRE(!(a != a));
    REQUIRE(a < b);
    REQUIRE(!(a < a));
    REQUIRE(!(a > b));
    REQUIRE(!(a > a));
    REQUIRE(a <= b);
    REQUIRE(a <= a);
    REQUIRE(!(a >= b));
    REQUIRE(a >= a);
  }

  SECTION("Handling nullopt")
  {
    NonNegOpt<int> a = 4;
    CHECK(!(a == std::nullopt));
    CHECK(!(std::nullopt == a));
    CHECK(a != std::nullopt);
    CHECK(std::nullopt != a);
    CHECK(!(a < std::nullopt));
    CHECK(std::nullopt < a);
    CHECK(a > std::nullopt);
    CHECK(!(std::nullopt > a));
    CHECK(!(a <= std::nullopt));
    CHECK(std::nullopt <= a);
    CHECK(a >= std::nullopt);
    CHECK(!(std::nullopt >= a));

    a = {};
    CHECK(a == std::nullopt);
    CHECK(std::nullopt == a);
    CHECK(!(a != std::nullopt));
    CHECK(!(std::nullopt != a));
    CHECK(!(a < std::nullopt));
    CHECK(!(std::nullopt < a));
    CHECK(!(a > std::nullopt));
    CHECK(!(std::nullopt > a));
    CHECK(a <= std::nullopt);
    CHECK(std::nullopt <= a);
    CHECK(a >= std::nullopt);
    CHECK(std::nullopt >= a);
  }

  SECTION("Optional vs Value Comparisons")
  {
    NonNegOpt<int> a = 4;

    REQUIRE(!(a == 1));
    REQUIRE(!(1 == a));
    REQUIRE(a != 1);
    REQUIRE(1 != a);
    REQUIRE(!(a < 1));
    REQUIRE(1 < a);
    REQUIRE(a > 1);
    REQUIRE(!(1 > a));
    REQUIRE(!(a <= 1));
    REQUIRE(1 <= a);
    REQUIRE(a >= 1);
    REQUIRE(!(1 >= a));

    REQUIRE(a == 4);
    REQUIRE(4 == a);
    REQUIRE(!(a != 4));
    REQUIRE(!(4 != a));
    REQUIRE(!(a < 4));
    REQUIRE(!(4 < a));
    REQUIRE(!(a > 4));
    REQUIRE(!(4 > a));
    REQUIRE(a <= 4);
    REQUIRE(4 <= a);
    REQUIRE(a >= 4);
    REQUIRE(4 >= a);
  }

  SECTION("String Comparison - Same Optional Type")
  {
    OptString a{"123"};
    OptString b{"456"};

    REQUIRE(!(a == b));
    REQUIRE(a == a);
    REQUIRE(a != b);
    REQUIRE(!(a != a));
    REQUIRE(a < b);
    REQUIRE(!(a < a));
    REQUIRE(!(a > b));
    REQUIRE(!(a > a));
    REQUIRE(a <= b);
    REQUIRE(a <= a);
    REQUIRE(!(a >= b));
    REQUIRE(a >= a);
  }

  SECTION("nullopt Comparison with Complex Type")
  {
    const OptString a{"123"};
    OptStringView   b{"456"};

    REQUIRE(!(a == b));
    REQUIRE(a == a);
    REQUIRE(a != b);
    REQUIRE(!(a != a));
    REQUIRE(a < b);
    REQUIRE(!(a < a));
    REQUIRE(!(a > b));
    REQUIRE(!(a > a));
    REQUIRE(a <= b);
    REQUIRE(a <= a);
    REQUIRE(!(a >= b));
    REQUIRE(a >= a);
  }
}
