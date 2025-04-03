#pragma once

#include <array>
#include <cassert>
#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_tostring.hpp>
#include <climits>
#include <compare>
#include <cstdio>
#include <stdexcept>
#include <zxshady/optional.hpp>

template<typename T, typename Interface>
struct Catch::StringMaker<zxshady::tombstone_optional<T, Interface>> {
  static std::string convert(const zxshady::tombstone_optional<T, Interface>& o)
  {
    if (!o)
      return "std::nullopt";
    else
      return Catch::StringMaker<T>::convert(*o);
  }
};

template<typename T>
struct Interface;


template<typename T>
struct ZeroBitPatternInterface {
  static_assert(std::is_trivially_copyable_v<T>, "");
  static bool is_null(const T& x) noexcept
  {
    static const auto zero = []() {
      T a;
      std::memset(&a, 0, sizeof(T));
      return a;
    }();

    return std::memcmp(&x, &zero, sizeof(x)) == 0;
  }
  static void initialize_null_state(T& x) noexcept { std::memset(&x, 0, sizeof(x)); }
};


struct DefaultConstructorInterface {
  template<typename T>
  static bool is_null(const T& x) noexcept
  {
    return x == T{};
  }
  template<typename T>
  static void initialize_null_state(T& x) noexcept
  {
    ::new (&x) T();
  }
  template<typename T>
    requires(!std::is_trivially_destructible_v<T>)
  static void destroy_null_state(T& x) noexcept
  {
    x.~T();
  }
};

template<typename String>
struct StringSetToNullInterface {
  static bool is_null(const String& x) noexcept { return x == "[std::nullopt]"; }
  static void initialize_null_state(String& x) noexcept { ::new (&x) String("[std::nullopt]"); }
  static void destroy_null_state(String& x) noexcept { x.~String(); }
};

using OptString     = zxshady::tombstone_optional<std::string, StringSetToNullInterface<std::string>>;
using OptStringView = zxshady::tombstone_optional<std::string_view, StringSetToNullInterface<std::string_view>>;


template<typename T>
struct NonNeg {
  constexpr NonNeg(T x) : value(x < 0 ? throw std::invalid_argument("Negative input not allowed") : x) {}
  template<typename U>
    requires requires(U u) { T{u}; }
  constexpr NonNeg(const NonNeg<U> x) noexcept : NonNeg(x.get_value())
  {
  }
  constexpr T    get_value() const { return value; }
  constexpr auto operator<=>(const NonNeg&) const = default;
private:
  T value;


  friend struct Interface<NonNeg>;
};

template<>
struct Interface<bool> {
  static constexpr unsigned char secret_value = 0b0000'0010;
  static_assert(sizeof(unsigned char) == sizeof(bool), "");
  static bool is_null(bool x) noexcept { return std::memcmp(&x, &secret_value, sizeof(x)) == 0; }
  static void initialize_null_state(bool& x) noexcept { std::memcpy(&x, &secret_value, sizeof(x)); }
  //static void destroy_null_state(T& x) noexcept { }
};

template<typename T>
struct Interface<NonNeg<T>> {
  static constexpr bool is_null(NonNeg<T> x) noexcept { return x.value == -1; }
  static constexpr void initialize_null_state(NonNeg<T>& x) noexcept
  {
    std::construct_at(&x, NonNeg<T>(0));
    x.value = -1;
  }
  //static void destroy_null_state(T& x) noexcept { }
};


template<typename T>
using IOpt = zxshady::tombstone_optional<T, Interface<T>>;


template<typename T>
using NonNegOpt = IOpt<NonNeg<T>>;

template<typename T>
struct Catch::StringMaker<NonNeg<T>> {
  static std::string convert(NonNeg<T> o) { return Catch::StringMaker<T>::convert(o.get_value()); }
};