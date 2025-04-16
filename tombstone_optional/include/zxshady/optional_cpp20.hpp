#pragma once

#include <compare>
#include <concepts>
#include <initializer_list>
#include <memory>   // std::addressof std::construct_at
#include <optional> // std::hash is in here
#include <type_traits>
#include <zxshady/optional_fwd.hpp>
#ifndef ZXSHADY_OPTIONAL_TOMBSTONE_ASSERT
  #include <cassert>

  #define ZXSHADY_OPTIONAL_TOMBSTONE_ASSERT(x, msg, ...) assert(x&& msg)
#endif
#ifndef ZXFWD
  #define ZXFWD(x) static_cast<decltype(x)&&>(x)
#endif

namespace zxshady {

namespace concepts {
  template<typename Traits, typename Type>
  concept tombstone_traits_for = std::is_empty_v<Traits> && requires(Type type, const Type ctype) {
    { Traits::is_null(ctype) } noexcept -> std::same_as<bool>;
    { Traits::initialize_null_state(type) } noexcept;
  };

  template<typename Traits, typename Type>
  concept tombstone_trivial_destroy_traits_for = tombstone_traits_for<Traits, Type> &&
    (!requires(Type& type) { Traits::destroy_null_state(type); });

} // namespace concepts


namespace tombstone_optional_details {
  template<typename T>
  concept CopyConstructible = std::is_copy_constructible_v<T>;
  template<typename T>
  concept CopyAssignable = std::is_copy_assignable_v<T>;

  template<typename T>
  concept MoveConstructible = std::is_move_constructible_v<T>;
  template<typename T>
  concept MoveAssignable = std::is_move_assignable_v<T>;

  template<typename T>
  concept TriviallyCopyConstructible = CopyConstructible<T> && std::is_trivially_copy_constructible_v<T>;

  template<typename T>
  concept TriviallyCopyAssignable = CopyAssignable<T> && std::is_trivially_copy_assignable_v<T>;

  template<typename T>
  concept TriviallyMoveConstructible = MoveConstructible<T> && std::is_trivially_move_constructible_v<T>;

  template<typename T>
  concept TriviallyMoveAssignable = MoveAssignable<T> && std::is_trivially_move_assignable_v<T>;


  template<typename T, typename Traits>
  void TombstoneOptionalTest(zxshady::tombstone_optional<T, Traits>**);

  template<typename T, typename Traits>
  void TombstoneOptionalConvertibleTest(const zxshady::tombstone_optional<T, Traits>&);


  template<typename T>
  concept TombstoneOptional = requires(std::remove_cv_t<T>** u) { TombstoneOptionalTest(u); };

  template<typename T>
  concept TombstoneOptionalConvertible = requires(T u) { TombstoneOptionalConvertibleTest(u); };

} // namespace tombstone_optional_details


template<auto Value>
struct tombstone_value_pattern {
private:
  using type = decltype(Value);
  static_assert(std::is_nothrow_destructible_v<type>);
  static_assert(std::is_nothrow_constructible_v<type, type>);
  static_assert(requires(const type& t) {
    { t == t } noexcept -> std::convertible_to<bool>;
  });


public:
  static void initialize_null_state(type& x) noexcept { ::new (&x) type(Value); }
  static bool is_null(const type& x) noexcept { return x == Value; }

  static void destroy_null_state(type& x) noexcept
    requires(!std::is_trivially_destructible_v<type>)
  {
    x.~type();
  }
};

template<>
struct tombstone_traits<bool> {
  static constexpr unsigned char null_value = 0xff;
  static void                    initialize_null_state(bool& x) noexcept { ::new (&x) unsigned char(null_value); }
  static bool is_null(const bool& x) noexcept { return reinterpret_cast<const unsigned char&>(x) == null_value; }
};

template<typename T, typename Traits>
class tombstone_optional {
  static_assert(std::is_nothrow_destructible_v<T>, "T must be no throw destructible");
  static_assert(concepts::tombstone_traits_for<Traits, T>, "Traits must be a tombstone_traits class for T");
  static_assert(
    concepts::tombstone_trivial_destroy_traits_for<Traits, T> ||
      requires(T t) {
        { Traits::destroy_null_state(t) } noexcept;
      },
    "Traits::destroy_null_state must be noexcept or not defined to be declared as trivial!");
public:
  using value_type  = T;
  using traits_type = Traits;

  constexpr tombstone_optional() noexcept { Traits::initialize_null_state(mValue); }
  constexpr tombstone_optional(std::nullopt_t) noexcept : tombstone_optional() {}

  template<typename U = std::remove_cv_t<T>>
    requires std::constructible_from<T, U>
  explicit(!std::is_convertible_v<U, T>) constexpr tombstone_optional(U&& u) noexcept(std::is_nothrow_constructible_v<T, U>)
  : mValue(ZXFWD(u))
  {
    ZXSHADY_OPTIONAL_TOMBSTONE_ASSERT(!Traits::is_null(mValue),
                                      "\"u\" cannot be the null state value for `zxshady::optional_tombstone`",
                                      mValue);
  }

  template<typename U, typename... Args>
    requires std::constructible_from<T, std::initializer_list<U>, Args...>
  explicit constexpr tombstone_optional(std::in_place_t, std::initializer_list<U> ilist, Args&&... args) noexcept(
    std::is_nothrow_constructible_v<T, Args...>)
  : mValue(ilist, ZXFWD(args)...)
  {
    ZXSHADY_OPTIONAL_TOMBSTONE_ASSERT(!Traits::is_null(mValue),
                                      "T(args...) cannot be the null state value for `zxshady::optional_tombstone`",
                                      mValue);
  }

  template<typename... Args>
    requires std::constructible_from<T, Args...>
  explicit constexpr tombstone_optional(std::in_place_t, Args&&... args) noexcept(std::is_nothrow_constructible_v<T, Args...>)
  : mValue(ZXFWD(args)...)
  {
    ZXSHADY_OPTIONAL_TOMBSTONE_ASSERT(!Traits::is_null(mValue),
                                      "T(args...) cannot be the null state value for `zxshady::optional_tombstone`",
                                      mValue);
  }

  constexpr tombstone_optional(const tombstone_optional&)
    requires tombstone_optional_details::TriviallyCopyConstructible<T> && concepts::tombstone_trivial_destroy_traits_for<Traits, T>
  = default;
  constexpr tombstone_optional(tombstone_optional&&)
    requires tombstone_optional_details::TriviallyMoveConstructible<T> && concepts::tombstone_trivial_destroy_traits_for<Traits, T>
  = default;
  constexpr tombstone_optional& operator=(const tombstone_optional&)
    requires tombstone_optional_details::TriviallyCopyAssignable<T> && concepts::tombstone_trivial_destroy_traits_for<Traits, T>
  = default;
  constexpr tombstone_optional& operator=(tombstone_optional&&)
    requires tombstone_optional_details::TriviallyMoveAssignable<T> && concepts::tombstone_trivial_destroy_traits_for<Traits, T>
  = default;
  constexpr ~tombstone_optional()
    requires std::is_trivially_destructible_v<T> && concepts::tombstone_trivial_destroy_traits_for<Traits, T>
  = default;

  // apparently these are needed even though it should be implicit from the contrain requirement not being satifified
  // but traits like `is_copy_constructible` will consider the U&& constructor as a copy constructor...
  // so lets be safe and delete them explicitly
  tombstone_optional(const tombstone_optional&)            = delete;
  tombstone_optional(tombstone_optional&&)                 = delete;
  tombstone_optional& operator=(const tombstone_optional&) = delete;
  tombstone_optional& operator=(tombstone_optional&&)      = delete;

  constexpr tombstone_optional(const tombstone_optional& that) noexcept(std::is_nothrow_copy_constructible_v<T>)
    requires tombstone_optional_details::CopyConstructible<T>
  {
    if (that)
      std::construct_at(std::addressof(mValue), that.mValue);
    else
      Traits::initialize_null_state(mValue);
  }

  constexpr tombstone_optional(tombstone_optional&& that) noexcept(std::is_nothrow_move_constructible_v<T>)
    requires tombstone_optional_details::MoveConstructible<T>
  {
    if (that)
      std::construct_at(std::addressof(mValue), std::move(that.mValue));
    else
      Traits::initialize_null_state(mValue);
  }

  constexpr tombstone_optional& operator=(const tombstone_optional& that) noexcept(std::is_nothrow_copy_assignable_v<T>)
    requires tombstone_optional_details::CopyAssignable<T> //&& (!concepts::tombstone_trivial_destroy_traits_for<Traits,T>)
  {
    if (that.has_value())
      Assign(that.mValue);
    else
      reset();
    return *this;
  }

  constexpr tombstone_optional& operator=(tombstone_optional&& that) noexcept(std::is_nothrow_move_assignable_v<T>)
    requires tombstone_optional_details::MoveAssignable<T> //&& (!concepts::tombstone_trivial_destroy_traits_for<Traits,T>)
  {
    if (that.has_value())
      Assign(std::move(that.mValue));
    else
      reset();
    return *this;
  }

  constexpr tombstone_optional& operator=(std::nullopt_t) noexcept
  {
    reset();
    return *this;
  }

  template<typename U = std::remove_cv_t<T>>
    requires std::constructible_from<T, U> && (!tombstone_optional_details::TombstoneOptional<std::remove_cvref_t<U>>) &&
    (!std::same_as<std::remove_cvref_t<U>, std::in_place_t>) && std::is_assignable_v<T&, U> &&
    (!std::is_scalar_v<T> || !std::same_as<std::decay_t<U>, T>)
  constexpr tombstone_optional& operator=(U&& value) noexcept
  {
    Assign(ZXFWD(value));
    return *this;
  }

  constexpr ~tombstone_optional() noexcept
  {
    if (has_value())
      mValue.~T();
    else if constexpr (!trivial_null_destroyer)
      Traits::destroy_null_state(mValue);
  }


  constexpr void reset() noexcept
  {
    if (has_value()) {
      mValue.~T();
      Traits::initialize_null_state(mValue);
    }
  }

  template<typename... Args>
    requires std::constructible_from<T, Args...>
  constexpr T& emplace(Args&&... args) noexcept(std::is_nothrow_constructible_v<T, Args...>)
  {
    if (has_value())
      mValue.~T();
    else if constexpr (!trivial_null_destroyer)
      Traits::destroy_null_state(mValue);

    std::construct_at(std::addressof(mValue), ZXFWD(args)...);
    ZXSHADY_OPTIONAL_TOMBSTONE_ASSERT(!Traits::is_null(mValue),
                                      "Setting null value in emplace uninteded use `.reset()` instead",
                                      mValue);
    return mValue;
  }

  template<typename U, typename... Args>
    requires std::constructible_from<T, std::initializer_list<U>, Args...>
  constexpr T& emplace(std::initializer_list<U> ilist, Args&&... args) noexcept(noexcept(T(ilist, ZXFWD(args)...)))
  {
    if (has_value())
      mValue.~T();
    else if constexpr (!trivial_null_destroyer)
      Traits::destroy_null_state(mValue);
    std::construct_at(std::addressof(mValue), ilist, ZXFWD(args)...);
    ZXSHADY_OPTIONAL_TOMBSTONE_ASSERT(!Traits::is_null(mValue),
                                      "Setting null value in emplace uninteded use `.reset()` instead",
                                      mValue);
    return mValue;
  }


  [[nodiscard]] constexpr auto&& operator*() & noexcept
  {
    ZXSHADY_OPTIONAL_TOMBSTONE_ASSERT(has_value(), "Calling operator* on empty optional!");
    return mValue;
  }
  [[nodiscard]] constexpr auto&& operator*() const& noexcept
  {
    ZXSHADY_OPTIONAL_TOMBSTONE_ASSERT(has_value(), "Calling operator* on empty optional!");
    return mValue;
  }

  [[nodiscard]] constexpr auto&& operator*() && noexcept
  {
    ZXSHADY_OPTIONAL_TOMBSTONE_ASSERT(has_value(), "Calling operator* on empty optional!");
    return static_cast<T&&>(mValue);
  }
  [[nodiscard]] constexpr auto&& operator*() const&& noexcept
  {
    ZXSHADY_OPTIONAL_TOMBSTONE_ASSERT(has_value(), "Calling operator* on empty optional!");
    return static_cast<const T&&>(mValue);
  }

  [[nodiscard]] constexpr auto&& value() &
  {
    if (!has_value())
      throw std::bad_optional_access();
    return mValue;
  }
  [[nodiscard]] constexpr const T& value() const&
  {
    if (!has_value())
      throw std::bad_optional_access();
    return mValue;
  }

  [[nodiscard]] constexpr T&& value() &&
  {
    if (!has_value())
      throw std::bad_optional_access();
    return static_cast<T&&>(mValue);
  }
  [[nodiscard]] constexpr const T&& value() const&&
  {
    if (!has_value())
      throw std::bad_optional_access();
    return static_cast<const T&&>(mValue);
  }

  template<typename U = std::remove_cv_t<T>>
  [[nodiscard]] constexpr T value_or(U&& default_value) const& noexcept(std::is_nothrow_constructible_v<T, U>)
  {
    return has_value() ? mValue : static_cast<T>(ZXFWD(default_value));
  }
  template<typename U = std::remove_cv_t<T>>
  [[nodiscard]] constexpr T value_or(U&& default_value) && noexcept(std::is_nothrow_constructible_v<T, U>)
  {
    return has_value() ? std::move(mValue) : static_cast<T>(ZXFWD(default_value));
  }

  [[nodiscard]] constexpr T*       operator->() noexcept { return std::addressof(**this); }
  [[nodiscard]] constexpr const T* operator->() const noexcept { return std::addressof(**this); }

  [[nodiscard]] constexpr explicit operator bool() const noexcept { return has_value(); }
  [[nodiscard]] constexpr bool     has_value() const noexcept { return !Traits::is_null(mValue); }

  constexpr friend void swap(tombstone_optional& a, tombstone_optional& b) noexcept(
    std::is_nothrow_move_constructible_v<T> && std::is_nothrow_swappable_v<T>)
  {
    // An optimization
    /*
    constexpr bool is_easily_swappable = trivial_null_destroyer && std::is_trivially_destructible_v<T> &&
      std::is_trivially_move_constructible_v<T> && std::is_trivially_move_assignable_v<T> &&
      !requires(T& a) { swap(a, a); };
*/

    const bool has_val = a.has_value();
    if (has_val == b.has_value()) {
      if (has_val) {
        using std::swap;
        swap(a.mValue, b.mValue);
      }
    }
    else {
      tombstone_optional& source = has_val ? a : b;
      tombstone_optional& null   = has_val ? b : a;
      if constexpr (!trivial_null_destroyer)
        Traits::destroy_null_state(null.mValue);
      std::construct_at(std::addressof(null.mValue), std::move(source.mValue));
      source.reset();
    }
  }
private:
  static constexpr bool trivial_null_destroyer = concepts::tombstone_trivial_destroy_traits_for<Traits, T>;

  template<typename U>
  constexpr void Assign(U&& u) noexcept(std::is_nothrow_assignable_v<T, U> && std::is_nothrow_constructible_v<T, U>)
  {
    if (has_value()) {
      mValue = ZXFWD(u);
    }
    else {
      if constexpr (!trivial_null_destroyer)
        Traits::destroy_null_state(mValue);
      std::construct_at(std::addressof(mValue), ZXFWD(u));
    }
    ZXSHADY_OPTIONAL_TOMBSTONE_ASSERT(!Traits::is_null(mValue),
                                      "Cannot set an optional with the null value! use .reset instead",
                                      mValue);
  }

  template<typename U, typename UTraits>
  friend class tombstone_optional;

  union {
    std::remove_cv_t<T> mValue;
  };
};


template<typename T, typename Traits>
[[nodiscard]] constexpr bool operator==(const tombstone_optional<T, Traits>& a, std::nullopt_t) noexcept
{
  return !a.has_value();
}

template<typename T, typename Traits, typename U>
[[nodiscard]] constexpr bool operator==(const tombstone_optional<T, Traits>& a, const U& b) noexcept(noexcept(*a == b))
  requires requires { *a == b; }
{
  return a.has_value() && *a == b;
}

template<std::equality_comparable T, typename Traits>
[[nodiscard]] constexpr bool operator==(const tombstone_optional<T, Traits>& a,
                                        const tombstone_optional<T, Traits>& b) noexcept(noexcept(*a == *b))
{
  return a.has_value() && b.has_value() && *a == *b;
}

// Order here is important && is conjunction token
template<typename T, typename Traits, typename U>
  requires(!tombstone_optional_details::TombstoneOptionalConvertible<U>) && std::three_way_comparable_with<U, T>
[[nodiscard]] constexpr std::compare_three_way_result_t<T, U> operator<=>(const tombstone_optional<T, Traits>& a,
                                                                          const U& b) noexcept(noexcept(*a <=> b))
{
  return a.has_value() ? *a <=> b : std::strong_ordering::less;
}

template<std::three_way_comparable T, typename Traits>
[[nodiscard]] constexpr std::strong_ordering operator<=>(const tombstone_optional<T, Traits>& a, std::nullopt_t) noexcept
{
  return a.has_value() <=> false;
}


template<typename T, typename Traits, std::three_way_comparable_with<T> U, typename UTraits>
[[nodiscard]] constexpr std::compare_three_way_result_t<T, U> operator<=>(
  const tombstone_optional<T, Traits>&  a,
  const tombstone_optional<U, UTraits>& b) noexcept(noexcept(*a <=> *b))
{
  const bool a_has_value = a.has_value();
  const bool b_has_value = b.has_value();
  return a_has_value && b_has_value ? *a <=> *b : a_has_value <=> b_has_value;
}


} // namespace zxshady


namespace std {
template<typename T, typename Traits>
  requires requires(const T& t) {
    { std::hash<T>()(t) } noexcept -> std::same_as<std::size_t>;
  }
struct hash<zxshady::tombstone_optional<T, Traits>> {
  constexpr size_t operator()(const zxshady::tombstone_optional<T, Traits>& opt) const noexcept(noexcept(hash<T>()(*opt)))
  {
    return opt ? hash<T>()(*opt) : 0;
  }
};

} // namespace std