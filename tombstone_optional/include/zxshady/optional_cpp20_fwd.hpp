#pragma once

namespace zxshady {

template<typename T, typename = void>
struct tombstone_traits;

template<auto Value>
struct tombstone_value_pattern;

template<typename T, typename Traits = tombstone_traits<T>>
class tombstone_optional;

template<typename T, auto Value>
using optional_via_senitiel = tombstone_optional<T, tombstone_value_pattern<Value>>;

}; // namespace zxshady