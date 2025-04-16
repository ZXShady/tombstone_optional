#include "interface.hpp"
#include <utility>

TEST_CASE("Constexpr", "[constexpr]")
{
  SECTION("Default initializing")
  {
    constexpr NonNegOpt<int> o1{};
    constexpr NonNegOpt<int> o2 = {};
    constexpr NonNegOpt<int> o3 = std::nullopt;
    constexpr NonNegOpt<int> o4 = {std::nullopt};
    constexpr NonNegOpt<int> o5(std::nullopt);
    constexpr NonNegOpt<int> o6{std::nullopt};

    STATIC_REQUIRE(!o1.has_value());
    STATIC_REQUIRE(!o2.has_value());
    STATIC_REQUIRE(!o3.has_value());
    STATIC_REQUIRE(!o4.has_value());
    STATIC_REQUIRE(!o5.has_value());
    STATIC_REQUIRE(!o6.has_value());
  }
  SECTION("Copying")
  {
    constexpr NonNegOpt<int> o1 = 42;
    constexpr NonNegOpt<int> o2{42};
    constexpr NonNegOpt<int> o3(42);
    constexpr NonNegOpt<int> o4 = {42};
    constexpr int            i  = 42;
    constexpr NonNegOpt<int> o5 = i;
    constexpr NonNegOpt<int> o6 = o1;
    constexpr NonNegOpt<int> o7 = {o1};
    constexpr NonNegOpt<int> o8{o1};
    constexpr NonNegOpt<int> o9(o1);

    STATIC_REQUIRE(*o1 == 42);
    STATIC_REQUIRE(*o2 == 42);
    STATIC_REQUIRE(*o3 == 42);
    STATIC_REQUIRE(*o4 == 42);
    STATIC_REQUIRE(*o5 == 42);
    STATIC_REQUIRE(*o6 == 42);
    STATIC_REQUIRE(*o7 == 42);
    STATIC_REQUIRE(*o8 == 42);
    STATIC_REQUIRE(*o9 == 42);
  }
}
