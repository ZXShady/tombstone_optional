#include "interface.hpp"
#include <utility>

TEST_CASE("Constexpr", "[constexpr]")
{
  SECTION("Default initializing")
  {
    constexpr NonNegOpt<int> o2{};
    constexpr NonNegOpt<int> o3 = {};
    constexpr NonNegOpt<int> o4 = std::nullopt;
    constexpr NonNegOpt<int> o5 = {std::nullopt};
    constexpr NonNegOpt<int> o6(std::nullopt);

    STATIC_REQUIRE(!o2);
    STATIC_REQUIRE(!o3);
    STATIC_REQUIRE(!o4);
    STATIC_REQUIRE(!o5);
    STATIC_REQUIRE(!o6);
  }
  SECTION("Copying")
  {
    constexpr NonNegOpt<int> o1 = 42;
    constexpr NonNegOpt<int> o2{42};
    constexpr NonNegOpt<int> o3(42);
    constexpr NonNegOpt<int> o4 = {42};
    constexpr int            i  = 42;
    constexpr NonNegOpt<int> o5 = std::move(i);
    constexpr NonNegOpt<int> o6{std::move(i)};
    constexpr NonNegOpt<int> o7(std::move(i));
    constexpr NonNegOpt<int> o8 = {std::move(i)};

    STATIC_REQUIRE(*o1 == 42);
    STATIC_REQUIRE(*o2 == 42);
    STATIC_REQUIRE(*o3 == 42);
    STATIC_REQUIRE(*o4 == 42);
    STATIC_REQUIRE(*o5 == 42);
    STATIC_REQUIRE(*o6 == 42);
    STATIC_REQUIRE(*o7 == 42);
    STATIC_REQUIRE(*o8 == 42);
  }
}
