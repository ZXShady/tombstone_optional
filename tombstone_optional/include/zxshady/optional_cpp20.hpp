#pragma once

#include <compare>
#include <concepts>
#include <initializer_list>
#include <memory>   // std::addressof std::construct_at
#include <optional> // std::hash is in here
#include <type_traits>
#ifndef ZXSHADY_OPTIONAL_TOMBSTONE_ASSERT
  #include <cassert>

  #define ZXSHADY_OPTIONAL_TOMBSTONE_ASSERT(x, msg, ...) assert(x&& msg)
#endif
#ifndef ZXFWD
  #define ZXFWD(x) static_cast<decltype(x)&&>(x)
#endif

namespace zxshady {

namespace concepts {
  template<typename Interface, typename Type>
  concept tombstone_optional_interface_for = std::is_empty_v<Interface> && requires(Type type, const Type ctype) {
    { Interface::is_null(ctype) } noexcept -> std::same_as<bool>;
    { Interface::initialize_null_state(type) } noexcept;
  };

  template<typename Interface, typename Type>
  concept tombstone_optional_trivial_destroy_interface_for = tombstone_optional_interface_for<Interface, Type> &&
    (!requires(Type& type) { Interface::destroy_null_state(type); });

} // namespace concepts


template<typename T, concepts::tombstone_optional_interface_for<T> Interface>
class tombstone_optional;


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


  template<typename T, typename Optional>
  concept UnwrapOptionalConstructor = !(
    std::is_constructible_v<T, Optional&> && std::is_constructible_v<T, const Optional&> &&
    std::is_constructible_v<T, Optional&&> && std::is_constructible_v<T, const Optional&&> &&
    std::is_convertible_v<Optional&, T> && std::is_convertible_v<const Optional&, T> &&
    std::is_convertible_v<Optional&&, T> && std::is_convertible_v<const Optional&&, T>);

  template<typename T, typename Optional>
  concept UnwrapOptionalAssign = UnwrapOptionalConstructor<T, Optional> &&
    !(std::is_assignable_v<T&, Optional&> && std::is_assignable_v<T&, const Optional&> &&
      std::is_assignable_v<T&, Optional&&> && std::is_assignable_v<T&, const Optional&&>);


  template<typename T>
  concept TombstoneOptional = requires(T** u) {
    []<typename U, typename UInterface>(tombstone_optional<U, UInterface>**) {}(u);
  };
} // namespace tombstone_optional_details


template<typename T, typename Interface = void>
using optional = std::conditional_t<std::is_void_v<Interface>, std::optional<T>, tombstone_optional<T, Interface>>;

template<typename T, concepts::tombstone_optional_interface_for<T> Interface>
class tombstone_optional {
  static_assert(std::is_nothrow_destructible_v<T>, "T must be nothrow destructible");
  static_assert(
    concepts::tombstone_optional_trivial_destroy_interface_for<Interface, T> ||
      requires(T t) {
        { Interface::destroy_null_state(t) } noexcept;
      },
    "Interface::destroy_null_state must be noexcept or not defined to be declared as trivial!");
public:
  using value_type = T;
  using interface  = Interface;

  constexpr tombstone_optional() noexcept { Interface::initialize_null_state(mValue); }
  constexpr tombstone_optional(std::nullopt_t) noexcept : tombstone_optional() {}

  template<typename U = std::remove_cv_t<T>>
    requires std::constructible_from<T, U>
  explicit(!std::is_convertible_v<U, T>) constexpr tombstone_optional(U&& u) noexcept(std::is_nothrow_constructible_v<T, U>)
  : mValue(ZXFWD(u))
  {
    ZXSHADY_OPTIONAL_TOMBSTONE_ASSERT(!Interface::is_null(mValue),
                                      "\"u\" cannot be the null state value for `zxshady::optional_tombstone`",
                                      mValue);
  }

  template<typename U, typename... Args>
    requires std::constructible_from<T, std::initializer_list<U>, Args...>
  explicit constexpr tombstone_optional(std::in_place_t, std::initializer_list<U> ilist, Args&&... args) noexcept(
    std::is_nothrow_constructible_v<T, Args...>)
  : mValue(ilist, ZXFWD(args)...)
  {
    ZXSHADY_OPTIONAL_TOMBSTONE_ASSERT(!Interface::is_null(mValue),
                                      "T(args...) cannot be the null state value for `zxshady::optional_tombstone`",
                                      mValue);
  }

  template<typename... Args>
    requires std::constructible_from<T, Args...>
  explicit constexpr tombstone_optional(std::in_place_t, Args&&... args) noexcept(std::is_nothrow_constructible_v<T, Args...>)
  : mValue(ZXFWD(args)...)
  {
    ZXSHADY_OPTIONAL_TOMBSTONE_ASSERT(!Interface::is_null(mValue),
                                      "T(args...) cannot be the null state value for `zxshady::optional_tombstone`",
                                      mValue);
  }

  template<typename U, typename UInterface>
    requires tombstone_optional_details::UnwrapOptionalConstructor<T, tombstone_optional<U, UInterface>>
  explicit(!std::is_convertible_v<const U&, T>) constexpr tombstone_optional(
    const tombstone_optional<U, UInterface>& that) noexcept(std::is_nothrow_constructible_v<T, const U&>)
  {
    if (that.has_value())
      std::construct_at(std::addressof(mValue), std::move(*that));
    else
      Interface::initialize_null_state(mValue);
  }

  constexpr tombstone_optional(const tombstone_optional&)
    requires tombstone_optional_details::TriviallyCopyConstructible<T> &&
    concepts::tombstone_optional_trivial_destroy_interface_for<Interface, T>
  = default;
  constexpr tombstone_optional(tombstone_optional&&)
    requires tombstone_optional_details::TriviallyMoveConstructible<T> &&
    concepts::tombstone_optional_trivial_destroy_interface_for<Interface, T>
  = default;
  constexpr tombstone_optional& operator=(const tombstone_optional&)
    requires tombstone_optional_details::TriviallyCopyAssignable<T> &&
    concepts::tombstone_optional_trivial_destroy_interface_for<Interface, T>
  = default;
  constexpr tombstone_optional& operator=(tombstone_optional&&)
    requires tombstone_optional_details::TriviallyMoveAssignable<T> &&
    concepts::tombstone_optional_trivial_destroy_interface_for<Interface, T>
  = default;
  constexpr ~tombstone_optional()
    requires std::is_trivially_destructible_v<T> && concepts::tombstone_optional_trivial_destroy_interface_for<Interface, T>
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
      Interface::initialize_null_state(mValue);
  }

  constexpr tombstone_optional(tombstone_optional&& that) noexcept(std::is_nothrow_move_constructible_v<T>)
    requires tombstone_optional_details::MoveConstructible<T>
  {
    if (that)
      std::construct_at(std::addressof(mValue), std::move(that.mValue));
    else
      Interface::initialize_null_state(mValue);
  }

  constexpr tombstone_optional& operator=(const tombstone_optional& that) noexcept(std::is_nothrow_copy_assignable_v<T>)
    requires tombstone_optional_details::CopyAssignable<T> //&& (!concepts::tombstone_optional_trivial_destroy_interface_for<Interface,T>)
  {
    if (that.has_value()) {
      if (has_value())
        mValue = that.mValue;
      else
        std::construct_at(std::addressof(mValue), that.mValue);
    }
    else {
      if (has_value())
        mValue.~T();
      else if constexpr (!trivial_null_destroyer)
        Interface::destroy_null_state(mValue);
    }
    return *this;
  }

  constexpr tombstone_optional& operator=(tombstone_optional&& that) noexcept(std::is_nothrow_move_assignable_v<T>)
    requires tombstone_optional_details::MoveAssignable<T> //&& (!concepts::tombstone_optional_trivial_destroy_interface_for<Interface,T>)
  {
    if (that.has_value()) {
      if (has_value())
        mValue = std::move(that.mValue);
      else
        std::construct_at(std::addressof(mValue), std::move(that.mValue));
    }
    else {
      if (has_value())
        mValue.~T();
      else if constexpr (!trivial_null_destroyer)
        Interface::destroy_null_state(mValue);
    }
    return *this;
  }

  constexpr tombstone_optional& operator=(std::nullopt_t) noexcept
  {
    reset();
    return *this;
  }


  template<typename U = std::remove_cv_t<T>>
    requires std::constructible_from<T, U> && (!tombstone_optional_details::TombstoneOptional<U>) &&
    (!std::same_as<std::remove_cvref_t<U>, std::in_place_t>) && std::is_assignable_v<T&, U> &&
    (!std::is_scalar_v<T> || !std::same_as<std::decay_t<U>, T>)
  constexpr tombstone_optional& operator=(U&& value) noexcept
  {

    if (has_value())
      mValue = ZXFWD(value);
    else
      std::construct_at(std::addressof(mValue), ZXFWD(value));
    ZXSHADY_OPTIONAL_TOMBSTONE_ASSERT(!Interface::is_null(mValue),
                                      "Cannot set an optional with the null value! use .reset instead",
                                      mValue);

    return *this;
  }
  template<typename U, typename UInterface>
    requires tombstone_optional_details::UnwrapOptionalAssign<T, tombstone_optional<U, UInterface>> &&
    std::constructible_from<T, const U&> && std::is_assignable_v<T&, const U&>
  constexpr tombstone_optional& operator=(const tombstone_optional<U, UInterface>& that) noexcept(noexcept(emplace(*that)))
  {
    if (that.has_value())
      emplace(*that);
    else
      reset();
    return *this;
  }

  template<typename U, typename UInterface>
    requires tombstone_optional_details::UnwrapOptionalAssign<T, tombstone_optional<U, UInterface>> &&
    std::constructible_from<T, U> && std::is_assignable_v<T&, U>
  constexpr tombstone_optional& operator=(tombstone_optional<U, UInterface>&& that) noexcept(
    noexcept(emplace(std::move(*that))))
  {
    if (that.has_value())
      emplace(std::move(*that));
    else
      reset();
    return *this;
  }

  constexpr ~tombstone_optional() noexcept
  {
    if (has_value())
      mValue.~T();

    if constexpr (!trivial_null_destroyer)
      Interface::destroy_null_state(mValue);
  }


  constexpr void reset() noexcept
  {
      if (has_value()) {
        mValue.~T();
        Interface::initialize_null_state(mValue);
    }
  }

  template<typename... Args>
    requires std::constructible_from<T, Args...>
  constexpr T& emplace(Args&&... args) noexcept(noexcept(T(ZXFWD(args)...)))
  {
    destroyNoNullState();
    std::construct_at(std::addressof(mValue), ZXFWD(args)...);
    ZXSHADY_OPTIONAL_TOMBSTONE_ASSERT(!Interface::is_null(mValue),
                                      "Setting null value in emplace uninteded use `.reset()` instead",
                                      mValue);
    return mValue;
  }

  template<typename U, typename... Args>
    requires std::constructible_from<T, std::initializer_list<U>, Args...>
  constexpr T& emplace(std::initializer_list<U> ilist, Args&&... args) noexcept(noexcept(T(ilist, ZXFWD(args)...)))
  {
    destroyNoNullState();
    std::construct_at(std::addressof(mValue), ilist, ZXFWD(args)...);
    ZXSHADY_OPTIONAL_TOMBSTONE_ASSERT(!Interface::is_null(mValue),
                                      "Setting null value in emplace uninteded use `.reset()` instead",
                                      mValue);
    return mValue;
  }


  [[nodiscard]] constexpr T& operator*() & noexcept
  {
    ZXSHADY_OPTIONAL_TOMBSTONE_ASSERT(has_value(), "Calling operator* on empty optional!");
    return mValue;
  }
  [[nodiscard]] constexpr const T& operator*() const& noexcept
  {
    ZXSHADY_OPTIONAL_TOMBSTONE_ASSERT(has_value(), "Calling operator* on empty optional!");
    return mValue;
  }

  [[nodiscard]] constexpr T&& operator*() && noexcept
  {
    ZXSHADY_OPTIONAL_TOMBSTONE_ASSERT(has_value(), "Calling operator* on empty optional!");
    return static_cast<T&&>(mValue);
  }
  [[nodiscard]] constexpr const T&& operator*() const&& noexcept
  {
    ZXSHADY_OPTIONAL_TOMBSTONE_ASSERT(has_value(), "Calling operator* on empty optional!");
    return static_cast<const T&&>(mValue);
  }

  [[nodiscard]] constexpr T& value() &
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
  [[nodiscard]] constexpr bool     has_value() const noexcept { return !Interface::is_null(mValue); }

  constexpr void swap(tombstone_optional& that) noexcept(std::is_nothrow_move_constructible_v<T> &&
                                                         std::is_nothrow_swappable_v<T>)
  {
    // An optimization
    constexpr bool is_easily_swappable = trivial_null_destroyer && std::is_trivially_destructible_v<T> &&
      std::is_trivially_move_constructible_v<T> && std::is_trivially_move_assignable_v<T> &&
      !requires(T& a) { swap(a, a); };

    if constexpr (is_easily_swappable) {
      std::swap(mValue, that.mValue); // intentional std qualification
    }
    else {
      const bool has_val = has_value();
      if (has_val == that.has_value()) {
        if (has_val) {
          using std::swap;
          swap(mValue, that.mValue);
        }
      }
      else {
        tombstone_optional& source = has_val ? *this : that;
        tombstone_optional& dest   = has_val ? that : *this;
        std::construct_at(std::addressof(dest.mValue), std::move(source.mValue));
        source.reset();
      }
    }
  }
private:
  static constexpr bool trivial_null_destroyer = concepts::tombstone_optional_trivial_destroy_interface_for<Interface, T>;

  constexpr void destroyNoNullState()
  {
    if constexpr (!std::is_trivially_destructible_v<T>)
      if (has_value())
        mValue.~T();
  }

  template<typename U, concepts::tombstone_optional_interface_for<U> UInterface>
  friend class tombstone_optional;
  struct Dummy { // named to avoid a warning of putting an anoynomous struct in an anonmous union
  };
  union {
    Dummy               dummy;
    std::remove_cv_t<T> mValue;
  };
};



template<typename T, typename Interface>
[[nodiscard]] constexpr bool operator==(const tombstone_optional<T, Interface>& a, std::nullopt_t) noexcept
{
  return !a.has_value();
}

template<typename T, typename Interface, typename U>
[[nodiscard]] constexpr bool operator==(const tombstone_optional<T, Interface>& a, const U& b) noexcept(noexcept(*a == b))
  requires requires { *a == b; }
{
  return a.has_value() && *a == b;
}

template<typename T, typename Interface, typename U, typename UInterface>
[[nodiscard]] constexpr bool operator==(const tombstone_optional<T, Interface>&  a,
                                        const tombstone_optional<U, UInterface>& b) noexcept(noexcept(*a == *b))
  requires requires { *a == *b; }
{
  return a.has_value() && b.has_value() && *a == *b;
}

template<typename T, typename Interface, typename U>
[[nodiscard]] constexpr std::compare_three_way_result_t<T, U> operator<=>(const tombstone_optional<T, Interface>& a,
                                                                          const U& b) noexcept(noexcept(*a <=> b))
  requires requires { *a <=> b; }
{
  return a.has_value() ? *a <=> b : std::strong_ordering::less;
}

template<typename T, typename Interface>
[[nodiscard]] constexpr std::strong_ordering operator<=>(const tombstone_optional<T, Interface>& a, std::nullopt_t) noexcept
{
  return a.has_value() <=> false;
}


template<typename T, typename Interface, typename U, typename UInterface>
[[nodiscard]] constexpr std::compare_three_way_result_t<T, U> operator<=>(
  const tombstone_optional<T, Interface>&  a,
  const tombstone_optional<U, UInterface>& b) noexcept(noexcept(*a <=> b))
  requires requires { *a <=> *b; }
{
  const bool a_has_value = a.has_value();
  const bool b_has_value = b.has_value();
  return a_has_value && b_has_value ? *a <=> *b : a_has_value <=> b_has_value;
}


template<typename T, typename Interface>
constexpr void swap(tombstone_optional<T, Interface>& a, tombstone_optional<T, Interface>& b) noexcept(noexcept(a.swap(b)))
{
  a.swap(b);
}
} // namespace zxshady

template<typename T, typename Interface>
struct std::hash<zxshady::tombstone_optional<T, Interface>> {
  constexpr size_t operator()(const zxshady::tombstone_optional<T, Interface>& opt) const
    noexcept(noexcept(hash<T>()(*opt)))
  {
    return opt ? std::hash<T>()(*opt) : 0;
  }
};