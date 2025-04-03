#include "interface.hpp"
#include <vector>

struct Movable {
  Movable()               = default;
  Movable(const Movable&) = delete;
  Movable(Movable&&) noexcept {}
};

TEST_CASE("Test constructors of tombstone_optional", "[tombstone_optional][constructors]")
{
  SECTION("Default constructor")
  {
    OptString emptyOpt;
    REQUIRE(!emptyOpt);
  }

  SECTION("Construct with std::nullopt")
  {
    OptString nullOpt = std::nullopt;
    REQUIRE(!nullOpt);
  }

  SECTION("Construct with value")
  {
    OptString optWithValue = "Hello World";
    REQUIRE(*optWithValue == "Hello World");
  }

  SECTION("Copy constructor")
  {
    OptString originalOpt = "Hello World";
    OptString copiedOpt   = originalOpt;
    REQUIRE(*copiedOpt == "Hello World");
  }

  SECTION("Copy from empty object")
  {
    OptString emptyOpt;
    OptString copiedFromEmpty = emptyOpt;
    REQUIRE(!copiedFromEmpty);
  }

  SECTION("Move constructor")
  {
    OptString originalOpt = "Hello World";
    OptString movedOpt    = std::move(originalOpt);
    REQUIRE(*movedOpt == "Hello World");
  }

  SECTION("Conversion between OptString and OptStringView")
  {
    OptStringView optStringView = "Hello World View";
    REQUIRE(*optStringView == "Hello World View");

    STATIC_REQUIRE(!std::is_convertible_v<OptStringView, OptString>);
    STATIC_REQUIRE(std::is_convertible_v<OptString, OptStringView>);

    OptString convertedOpt{optStringView};
    REQUIRE(*convertedOpt == "Hello World View");

    OptString movedOpt{std::move(optStringView)};
    REQUIRE(*movedOpt == "Hello World View");
  }
}
