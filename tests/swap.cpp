#include "interface.hpp"

TEST_CASE("swap value", "[swap][value]")
{
  OptString a = "Hello";
  OptString b = "World";
  swap(a,b);
  REQUIRE(*a == "World");
  REQUIRE(*b == "Hello");
}
TEST_CASE("Swap value with null", "[swap][nullopt]")
{
  OptString a = "Is someone there?";
  OptString b = {};
  swap(a, b);
  REQUIRE(!a.has_value());
  REQUIRE(*b == "Is someone there?");
}

TEST_CASE("Swap null with value", "[swap][nullopt]")
{
  OptString a;
  OptString b = "I should be null";
  swap(a,b);
  REQUIRE(!b.has_value());
  REQUIRE(*a == "I should be null");
}
