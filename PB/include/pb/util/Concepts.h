#pragma once

#include <concepts>
#include <optional>

#include <boost/bimap/bimap.hpp>

#include <pb/util/Error.h>
#include <pb/util/Traits.h>

namespace PB {

template <typename T>
concept SerializationPrimitiveConcept =
    std::same_as<T, int> || std::same_as<T, bool> ||
    std::same_as<T, unsigned> || std::same_as<T, std::string> ||
    std::same_as<T, Path>;

template <typename T>
concept BimapConcept = requires(T t, typename T::left_value_type lkey,
                                typename T::right_value_type rkey) {
  {
    t.left.erase(lkey)
  } -> std::same_as<size_t>;
};

template <typename T>
concept StdMapsConcept =
    requires(T a, typename T::key_type k, typename T::mapped_type v) {
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
      a.insert({k, v});
    } && !BimapConcept<T>;

template <typename T>
concept ContainerConcept =
    requires(T a, typename T::value_type v) {
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
    } && !std::same_as<T, std::string> && !std::same_as<T, const std::string> &&
    !StdMapsConcept<T> && !BimapConcept<T>;

} // namespace PB