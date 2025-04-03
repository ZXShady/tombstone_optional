# tombstone_optional

`TombstoneOptional` is a lightweight, efficient class designed to represent an optional value.

The class however uses special "null" values instead of taking up additional memory, making it memory-efficient.

## Features

- **No Extra Memory Overhead**: The `tombstone_optional` type uses special "null" values and does not add extra memory usage compared to the wrapped type.
- **Customizable Null Behavior**: The class is designed to be flexible, allowing you to define custom behaviors when the optional value is "null".

## Usage Example

```cpp
#include <iostream>
#include <zxshady/optional.hpp>

struct MyType {
    int value;
};

struct MyTypeInterface {
  static bool is_null(MyType x) noexcept { return x.value == -1;}
  static void initialize_null_state(MyType& x) noexcept { ::new(&x) MyType{-1};}
}

int main() {
    zxshady::tombstone_optional<MyType,MyTypeInterface> optional_value;
    static_assert(sizeof(optional_value) == sizeof(MyType));
    optional_value = MyType{42};  // Set a value

    if (optional_value) {
        std::cout << "Value: " << optional_value->value << std::endl;
    }

    optional_value.reset();  // Reset to null

    if (!optional_value) {
        std::cout << "The value is now null!" << std::endl;
    }
}
```

# Methods

    reset(): Resets the tombstone_optional object to a "null" state.
    bool has_value();
    explicit operator bool(): Implicit conversion operator that checks if the object contains a value (i.e., not null).

    operator->(): Provides access to the underlying value if present. UB if `has_value()` returns false

    operator*(): Dereferences the object to access the underlying value. UB if `has_value()` returns false`

    value(); throws `bad_optional` if `has_value()` returns false otherwise equal to `operator*()`

    value_or(U&&) returns `operator*()` if `has_value()` is true otherwise returns its arguement


# Interfaces

Interfaces are the way the `tombstone_optional` know it is a `null` value they are defined like this

```cpp
struct Interface {
  static bool is_null(const T& x) noexcept { return x == T{};}
  static void initialize_null_state(T& x) noexcept { ::new(&x) T{};}
  static void destroy_null_state(T& x) noexcept { x.~T();}
}
```

**Note** that `initialize_null_state` take a reference to uninitialized memory so you need to placement new into it otherwise you will probably crash

`destroy_null_state` if not defined it will assume this interface is a *tombstone_optional_trivial_destroy_interface_for*
which means that destroying the `null` state does not do anything therefore it is trivial so if your type is trivial don't define it otherwise the `tombstone_optional` will not be trivially destructible.

# Concepts

There are 2 concepts in this library

    `tombstone_optional_interface_for<Interface,T>`: Whether this interface is valid i.e has `is_null` and `initialize_null_state` with the syntactic requirements

    tombstone_optional_trivial_destroy_interface_for<Interface,T>: same as `tombstone_optional_interface_for` except it also requires not defining `destroy_null_state`


## Provided Interfaces
  Coming Soon

  