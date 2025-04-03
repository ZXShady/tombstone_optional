#include "interface.hpp"

TEST_CASE("NonNegOpt assignment value tests", "[NonNegOpt][assignment]")
{
  NonNegOpt<int> o1 = 42;
  NonNegOpt<int> o2 = 12;
  NonNegOpt<int> o3;

  // Test self-assignment
  o1 = o1;
  REQUIRE(*o1 == 42);

  // Test assignment from another object
  o1 = o2;
  REQUIRE(*o1 == 12);

  // Test assignment from an empty object
  o1 = o3;
  REQUIRE(!o1);

  // Test assignment from a value
  o1 = 42;
  REQUIRE(*o1 == 42);

  // Test assignment from std::nullopt
  o1 = std::nullopt;
  o1.reset();
  REQUIRE(!o1);

  // Test assignment from {}
  o1 = {};
  REQUIRE(!o1);


  // Test assignment from a moved object
  o1 = std::move(o2);
  REQUIRE(*o1 == 12);
  NonNegOpt<short> o4 = 42;
  o1                  = o4;
  REQUIRE(*o1 == 42);

  // Test move assignment to another object
  o1 = std::move(o4);
  REQUIRE(*o1 == 42);
}
