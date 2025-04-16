#include "interface.hpp"

TEST_CASE("Noexcept", "[noexcept]")
{
  NonNegOpt<int> o1{4};

  SECTION("comparison with nullopt")
  {
    STATIC_REQUIRE(noexcept(o1 == std::nullopt));
    STATIC_REQUIRE(noexcept(std::nullopt == o1));
    STATIC_REQUIRE(noexcept(o1 != std::nullopt));
    STATIC_REQUIRE(noexcept(std::nullopt != o1));
    STATIC_REQUIRE(noexcept(o1 < std::nullopt));
    STATIC_REQUIRE(noexcept(std::nullopt < o1));
    STATIC_REQUIRE(noexcept(o1 <= std::nullopt));
    STATIC_REQUIRE(noexcept(std::nullopt <= o1));
    STATIC_REQUIRE(noexcept(o1 > std::nullopt));
    STATIC_REQUIRE(noexcept(std::nullopt > o1));
    STATIC_REQUIRE(noexcept(o1 >= std::nullopt));
    STATIC_REQUIRE(noexcept(std::nullopt >= o1));
  }

  SECTION("swap")
  {
    NonNegOpt<int> o2{42};
    STATIC_REQUIRE(noexcept(swap(o1, o2)));
  }

  SECTION("observers")
  {
    STATIC_REQUIRE(noexcept(static_cast<bool>(o1)));
    STATIC_REQUIRE(noexcept(o1.has_value()));
  }

  SECTION("modifiers") { STATIC_REQUIRE(noexcept(o1.reset())); }
}
