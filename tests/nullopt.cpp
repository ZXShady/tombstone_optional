#include "interface.hpp"

TEST_CASE("Nullopt", "[nullopt]")
{
  NonNegOpt<int> o1 = std::nullopt;
  NonNegOpt<int> o2{std::nullopt};
  NonNegOpt<int> o3(std::nullopt);
  NonNegOpt<int> o4 = {std::nullopt};

  REQUIRE(!o1);
  REQUIRE(!o2);
  REQUIRE(!o3);
  REQUIRE(!o4);
}
