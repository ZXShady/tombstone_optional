#include "interface.hpp"
#include <catch2/catch_template_test_macros.hpp>
// Structs moved to the outer scope

// Default constructible, copyable, movable
struct DefaultConstructible {
  DefaultConstructible()                                       = default;
  DefaultConstructible(const DefaultConstructible&)            = default;
  DefaultConstructible(DefaultConstructible&&)                 = default;
  DefaultConstructible& operator=(const DefaultConstructible&) = default;
  DefaultConstructible& operator=(DefaultConstructible&&)      = default;
  ~DefaultConstructible()                                      = default;
};

// Custom copy and move constructor, with default destructor
struct CustomCopyMove {
  CustomCopyMove() = default;

  CustomCopyMove(const CustomCopyMove&) {}
  CustomCopyMove& operator=(const CustomCopyMove&) { return *this; }
  CustomCopyMove(CustomCopyMove&&) {}
  CustomCopyMove& operator=(CustomCopyMove&&) { return *this; }
  ~CustomCopyMove() {}
};

// Default constructor, copyable, but only default move
struct OnlyDefaultMove {
  OnlyDefaultMove() {}
  OnlyDefaultMove(const OnlyDefaultMove&) {}
  OnlyDefaultMove& operator=(const OnlyDefaultMove&) { return *this; }
  OnlyDefaultMove(OnlyDefaultMove&&)            = default;
  OnlyDefaultMove& operator=(OnlyDefaultMove&&) = default;
  ~OnlyDefaultMove() {}
};

// Only default copy and move constructors and assignment
struct DefaultCopy {
  DefaultCopy(const DefaultCopy&)            = default;
  DefaultCopy(DefaultCopy&&)                 = default;
  DefaultCopy& operator=(const DefaultCopy&) = default;
  DefaultCopy& operator=(DefaultCopy&&)      = default;
  ~DefaultCopy()                             = default;
};

// All constructors and assignment deleted
struct DeleteAll {
  DeleteAll()                            = default;
  DeleteAll(const DeleteAll&)            = delete;
  DeleteAll(DeleteAll&&)                 = delete;
  DeleteAll& operator=(const DeleteAll&) = delete;
  DeleteAll& operator=(DeleteAll&&)      = delete;
};

// Custom move constructor with deleted copy constructor and assignment
struct CustomDeleteMove {
  CustomDeleteMove()                                   = default;
  CustomDeleteMove(const CustomDeleteMove&)            = delete;
  CustomDeleteMove(CustomDeleteMove&&)                 = default;
  CustomDeleteMove& operator=(const CustomDeleteMove&) = delete;
  CustomDeleteMove& operator=(CustomDeleteMove&&)      = default;
};

// Default copy constructor with deleted move constructor
struct DefaultCopyDeleteMove {
  DefaultCopyDeleteMove(const DefaultCopyDeleteMove&)            = default;
  DefaultCopyDeleteMove(DefaultCopyDeleteMove&&)                 = delete;
  DefaultCopyDeleteMove& operator=(const DefaultCopyDeleteMove&) = default;
  DefaultCopyDeleteMove& operator=(DefaultCopyDeleteMove&&)      = delete;
};

TEMPLATE_TEST_CASE("Traits",
                   "[traits]",
                   DefaultConstructible,
                   CustomCopyMove,
                   OnlyDefaultMove,
                   DefaultCopy,
                   DeleteAll,
                   CustomDeleteMove,
                   DefaultCopyDeleteMove)
{
  using T = TestType;
  using O = zxshady::tombstone_optional<T, ::DefaultConstructorInterface>;
  CHECK(std::is_nothrow_default_constructible<O>::value);
  CHECK(std::is_nothrow_constructible<O, std::nullopt_t>::value);

  CHECK(sizeof(T) == sizeof(O));
#define TRAIT_EQUAL(trait) CHECK(trait<T>::value == trait<O>::value)
  TRAIT_EQUAL(std::is_copy_constructible);
  TRAIT_EQUAL(std::is_move_constructible);

  if constexpr (zxshady::concepts::tombstone_optional_trivial_destroy_interface_for<typename O::interface, T>) {
    TRAIT_EQUAL(std::is_copy_assignable);
    TRAIT_EQUAL(std::is_move_assignable);
  }

  TRAIT_EQUAL(std::is_trivially_destructible);

  TRAIT_EQUAL(std::is_trivially_copy_constructible);
  TRAIT_EQUAL(std::is_trivially_move_constructible);
  if constexpr (zxshady::concepts::tombstone_optional_trivial_destroy_interface_for<typename O::interface, T>) {
    TRAIT_EQUAL(std::is_trivially_copy_assignable);
    TRAIT_EQUAL(std::is_trivially_move_assignable);
  }

#undef TRAIT_EQUAL
}