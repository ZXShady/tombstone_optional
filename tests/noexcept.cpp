#include "interface.hpp"

TEST_CASE("Noexcept", "[noexcept]")
{
  NonNegOpt<int> o1{4};
  NonNegOpt<int> o2{42};

  SECTION("comparison with nullopt")
  {
    REQUIRE(noexcept(o1 == std::nullopt));
    REQUIRE(noexcept(std::nullopt == o1));
    REQUIRE(noexcept(o1 != std::nullopt));
    REQUIRE(noexcept(std::nullopt != o1));
    REQUIRE(noexcept(o1 < std::nullopt));
    REQUIRE(noexcept(std::nullopt < o1));
    REQUIRE(noexcept(o1 <= std::nullopt));
    REQUIRE(noexcept(std::nullopt <= o1));
    REQUIRE(noexcept(o1 > std::nullopt));
    REQUIRE(noexcept(std::nullopt > o1));
    REQUIRE(noexcept(o1 >= std::nullopt));
    REQUIRE(noexcept(std::nullopt >= o1));
  }

  SECTION("swap")
  {
    REQUIRE(noexcept(swap(o1, o2)) == noexcept(o1.swap(o2)));

    struct nothrow_swappable {
      nothrow_swappable& swap(const nothrow_swappable&) noexcept { return *this; }
    };

    struct throw_swappable {
      throw_swappable() = default;
      throw_swappable(const throw_swappable&) {}
      throw_swappable(throw_swappable&&) {}
      throw_swappable& swap(const throw_swappable&) { return *this; }
    };

    std::optional<nothrow_swappable> nothrowswap;
    std::optional<throw_swappable>   throswap;
    REQUIRE(noexcept(nothrowswap.swap(nothrowswap)));
    REQUIRE(!noexcept(throswap.swap(throswap)));
  }

  SECTION("observers")
  {
    REQUIRE(noexcept(static_cast<bool>(o1)));
    REQUIRE(noexcept(o1.has_value()));
  }

  SECTION("modifiers") { 
      REQUIRE(noexcept(o1.reset()));
  }
}
