#include "interface.hpp"
#include <functional>
#include <unordered_set>

TEST_CASE("Hashing", "[hash]")
{

  SECTION("Hashing empty objects")
  {
    const std::hash<OptString> hash_fn{};

    const OptString   emptyOpt;
    const OptString   anotherEmptyOpt = {};
    const std::size_t hashValue       = hash_fn(emptyOpt);
    const std::size_t hashValue2      = hash_fn(anotherEmptyOpt);
    REQUIRE(hashValue == hashValue2);
  }

  SECTION("Hashing non-empty objects")
  {
    const OptString            nonEmptyOpt = "Hello";
    const std::hash<OptString> hash_fn{};
    const std::size_t          hashValue1 = hash_fn(nonEmptyOpt);

    OptString         anotherNonEmptyOpt = "Hello"; // intentionally not const
    const std::size_t hashValue2         = hash_fn(anotherNonEmptyOpt);
    REQUIRE(hashValue1 == hashValue2);

    const OptString   differentOpt = "World";
    const std::size_t hashValue3   = hash_fn(differentOpt);
    REQUIRE(hashValue1 != hashValue3);
  }

  SECTION("Hashing with std::unordered_set")
  {
    const OptString opt1 = "Test1";
    const OptString opt2 = "Test2";
    const OptString opt3 = opt1;

    std::unordered_set<OptString> optSet;
    optSet.insert(opt1);
    optSet.insert(opt2);
    optSet.insert(opt3);

    REQUIRE(optSet.size() == 2);
  }
}
