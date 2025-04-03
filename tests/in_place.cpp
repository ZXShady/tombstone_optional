#include "interface.hpp"
#include <utility>
#include <vector>
struct EmptyContainerInterface {
  template<typename C>
  static bool is_null(const C& c) noexcept
  {
    return c.empty();
  }
  template<typename C>
  static void initialize_null_state(C& c) noexcept
  {
    ::new (&c) C();
  }
  template<typename C>
  static void destroy_null_state(C& c) noexcept
  {
    c.~C();
  }
};

TEST_CASE("In place", "[in_place]")
{
  SECTION("Default constructing")
  {
    OptString o{std::in_place};
    static_assert(!std::is_convertible_v<std::in_place_t, OptString>);
    REQUIRE(o.has_value());
    REQUIRE(o->size() == 0);
    REQUIRE(o == "");
  }

  SECTION("Constructing with 1 arguement")
  {
    OptString o{std::in_place, "I am inplaced!"};
    REQUIRE(o.has_value());
    REQUIRE(o == "I am inplaced!");
  }

  SECTION("Constructing with std::initializer_list")
  {
    OptString o{std::in_place, {'Z', 'X', 'S', 'h', 'a', 'd', 'y'}};
    REQUIRE(o.has_value());
    REQUIRE(o == "ZXShady");
  }

  SECTION("Constructing with std::in_place")
  {
    NonNegOpt<int> o{std::in_place, 5};
    REQUIRE(o.has_value());
    REQUIRE(o == 5);
  }

  SECTION("Multiple arguements")
  {
    zxshady::tombstone_optional<std::pair<int, int>,DefaultConstructorInterface> o(std::in_place, 42, 21);
    REQUIRE(o);
    REQUIRE(o->first == 42);
    REQUIRE(o->second == 21);
  }

  SECTION("std::initializer_list arguement")
  {
    zxshady::tombstone_optional<std::vector<int>, EmptyContainerInterface> o(std::in_place, {42, 21, 22 / 7});
    REQUIRE(o);
    REQUIRE((*o)[0] == 42);
    REQUIRE((*o)[1] == 21);
    REQUIRE((*o)[2] == 22 / 7);
  }
}
