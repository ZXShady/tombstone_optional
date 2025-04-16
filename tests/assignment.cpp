#include "interface.hpp"

TEST_CASE("NonNegOpt assignment behavior", "[NonNegOpt][assignment]")
{
  SECTION("Self-assignment")
  {
    NonNegOpt<int> o = 42;
    o                = o;
    REQUIRE(*o == 42);
  }

  SECTION("Assignment from another NonNegOpt")
  {
    NonNegOpt<int> o1 = 42;
    NonNegOpt<int> o2 = 12;
    o1                = o2;
    REQUIRE(*o1 == 12);
  }

  SECTION("Assignment from empty NonNegOpt")
  {
    NonNegOpt<int> o1 = 42;
    NonNegOpt<int> o_empty;
    o1 = o_empty;
    REQUIRE(!o1);
  }

  SECTION("Assignment from a value")
  {
    NonNegOpt<int> o = 0;
    o                = 99;
    REQUIRE(*o == 99);
  }

  SECTION("Assignment from std::nullopt")
  {
    NonNegOpt<int> o = 42;
    o                = std::nullopt;
    REQUIRE(!o);
  }

  SECTION("Assignment from {}")
  {
    NonNegOpt<int> o = 42;
    o                = {};
    REQUIRE(!o);
  }

  SECTION("Move assignment from another object")
  {
    NonNegOpt<int> o1 = 0;
    NonNegOpt<int> o2 = 12;
    o1                = std::move(o2);
    REQUIRE(*o1 == 12);
  }
}

TEST_CASE("Bool optional assignment behavior", "[bool][assignment]")
{
  using BoolOpt = zxshady::tombstone_optional<bool>;

  REQUIRE(sizeof(BoolOpt) == sizeof(bool));

  SECTION("Self-assignment")
  {
    BoolOpt o = false;
    o         = o;
    REQUIRE(*o == false);
  }

  SECTION("Assignment from another optional")
  {
    BoolOpt       o1 = false;
    const BoolOpt o2 = true;
    o1               = o2;
    REQUIRE(*o1 == true);
  }

  SECTION("Assignment from empty optional")
  {
    BoolOpt       o1 = true;
    const BoolOpt o_empty;
    o1 = o_empty;
    REQUIRE(!o1.has_value());
  }

  SECTION("Assignment from value (true/false)")
  {
    BoolOpt o = false;
    o         = true;
    REQUIRE(*o == true);

    o = false;
    REQUIRE(*o == false);
  }

  SECTION("Assignment from std::nullopt")
  {
    BoolOpt o = true;
    o         = std::nullopt;
    REQUIRE(!o.has_value());
  }

  SECTION("Assignment from {}")
  {
    BoolOpt o = true;
    o         = {};
    REQUIRE(!o.has_value());
  }

  SECTION("Copy assignment")
  {
    BoolOpt       o1 = false;
    const BoolOpt o2 = true;
    o1               = o2;
    REQUIRE(*o1 == true);
  }

  SECTION("Move assignment")
  {
    BoolOpt o1 = false;
    BoolOpt o2 = true;
    o1         = std::move(o2);
    REQUIRE(*o1 == true);
  }
}

TEST_CASE("optional_with_value<int,-1> assignment behavior", "[NonNegOpt][assignment]")
{
  using Opt = zxshady::optional_via_senitiel<int, -1>;
  SECTION("Self-assignment")
  {
    Opt o = 42;
    o     = o;
    REQUIRE(*o == 42);
  }

  SECTION("Assignment from another NonNegOpt")
  {
    Opt o1 = 42;
    Opt o2 = 12;
    o1     = o2;
    REQUIRE(*o1 == 12);
  }

  SECTION("Assignment from empty NonNegOpt")
  {
    Opt o1 = 42;
    Opt o_empty;
    o1 = o_empty;
    REQUIRE(!o1);
  }

  SECTION("Assignment from a value")
  {
    Opt o = 0;
    o     = 99;
    REQUIRE(*o == 99);
  }

  SECTION("Assignment from std::nullopt")
  {
    Opt o = 42;
    o     = std::nullopt;
    REQUIRE(!o);
  }

  SECTION("Assignment from {}")
  {
    Opt o = 42;
    o     = {};
    REQUIRE(!o);
  }

  SECTION("Move assignment from another object")
  {
    Opt o1 = 0;
    Opt o2 = 12;
    o1     = std::move(o2);
    REQUIRE(*o1 == 12);
  }
}
