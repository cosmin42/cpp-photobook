#pragma once

#include <concepts>
#include <optional>

#include <pb/Error.h>
#include <pb/util/Traits.h>

namespace PB {

template <typename T>
concept SerializationPrimitiveConcept =
    std::same_as<T, int> || std::same_as<T, bool> ||
    std::same_as<T, std::string> || std::same_as<T, Path>;

template <typename T>
concept ContainerConcept = requires(T a, typename T::value_type v) {
  {
    a.begin()
  } -> std::same_as<typename T::iterator>;
  {
    a.end()
  } -> std::same_as<typename T::iterator>;
  {
    a.size()
  } -> std::convertible_to<std::size_t>;
  {
    a.empty()
  } -> std::same_as<bool>;
  a.insert(a.end(), v);
};

} // namespace PB