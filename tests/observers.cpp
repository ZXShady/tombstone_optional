#include "interface.hpp"

struct did_i_move {
  did_i_move() : yes(false) {};
  did_i_move(did_i_move&& that) { that.yes = true; }
  bool yes;
};

template<>
struct Interface<did_i_move> {
  static bool is_null(const did_i_move& x) noexcept { return Interface<bool>::is_null(x.yes); }
  static void initialize_null_state(did_i_move& x) noexcept
  {
    ::new (&x) did_i_move();
    Interface<bool>::initialize_null_state(x.yes);
  }
};


TEST_CASE("Observers", "[observers]")
{
  SECTION("Move constructing")
  {
    IOpt<did_i_move> source{std::in_place};
    did_i_move       value = std::move(source).value();
    REQUIRE(source->yes);
    REQUIRE(!value.yes);
  }
}
