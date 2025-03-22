#pragma once

#include <stdexcept>
#include <unordered_map>

#include <boost/functional/hash.hpp>

namespace infra {
template <typename K, typename V> class BijectiveUnorderedMap final {
private:
  std::unordered_map<K, V, boost::hash<K>> key_to_value;
  std::unordered_map<V, K, boost::hash<V>> value_to_key;

public:
  // Insert or update key-value pair
  void insert(const K &key, const V &value)
  {
    // Remove existing mappings if key or value already exist
    auto key_it = key_to_value.find(key);
    if (key_it != key_to_value.end()) {
      V old_value = key_it->second;
      value_to_key.erase(old_value);
    }

    auto value_it = value_to_key.find(value);
    if (value_it != value_to_key.end()) {
      K old_key = value_it->second;
      key_to_value.erase(old_key);
    }

    // Insert new mapping
    key_to_value[key] = value;
    value_to_key[value] = key;
  }

  // Remove by key
  void removeByKey(const K &key)
  {
    auto key_it = key_to_value.find(key);
    if (key_it != key_to_value.end()) {
      V value = key_it->second;
      key_to_value.erase(key_it);
      value_to_key.erase(value);
    }
  }

  // Remove by value
  void removeByValue(const V &value)
  {
    auto value_it = value_to_key.find(value);
    if (value_it != value_to_key.end()) {
      K key = value_it->second;
      value_to_key.erase(value_it);
      key_to_value.erase(key);
    }
  }

  // Lookup value by key
  V getValue(const K &key) const
  {
    auto it = key_to_value.find(key);
    if (it == key_to_value.end())
      throw std::out_of_range("Key not found.");
    return it->second;
  }

  // Lookup key by value
  K getKey(const V &value) const
  {
    auto it = value_to_key.find(value);
    if (it == value_to_key.end())
      throw std::out_of_range("Value not found.");
    return it->second;
  }

  // Check if key exists
  bool hasKey(const K &key) const { return key_to_value.count(key) > 0; }

  // Check if value exists
  bool hasValue(const V &value) const { return value_to_key.count(value) > 0; }

  // Clear the map
  void clear()
  {
    key_to_value.clear();
    value_to_key.clear();
  }

  // Size of the map
  size_t size() const { return key_to_value.size(); }
};
} // namespace infra
