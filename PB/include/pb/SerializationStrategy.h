#pragma once

#include <pb/persistence/Persistence.h>
#include <pb/util/Concepts.h>

namespace PB::Text {

template <typename T> std::variant<T, Error> deserialize(Json jsonData);

template <typename T>
std::variant<T, Error> deserialize(Json jsonData, std::string key,
                                   T defaultValue = T(), bool optional = false)
{
  if (jsonData.contains(key)) {
    return deserialize<T>(jsonData.at(key));
  }
  if (optional) {
    return defaultValue;
  }
  return Error() << ErrorCode::JSONParseError;
}

template <template <typename> typename Container, typename T>
std::variant<Container<T>, Error> deserialize(Json jsonData)
{
  auto         x = jsonData.dump();
  Container<T> result;
  for (const auto &stagedFolderJson : jsonData) {
    auto deserializedOrError = deserialize<T>(stagedFolderJson);
    if (std::holds_alternative<Error>(deserializedOrError)) {
      return std::get<Error>(deserializedOrError);
    }
    else {
      result.push_back(std::get<T>(deserializedOrError));
    }
  }
  return result;
}

template <template <typename> typename Container, typename T>
std::variant<Container<T>, Error>
deserialize(Json jsonData, std::string key,
            Container<T> defaultValue = Container<T>(), bool optional = false)
{
  if (jsonData.contains(key)) {
    return deserialize<Container, T>(jsonData.at(key));
  }
  if (optional) {
    return defaultValue;
  }
  return Error() << ErrorCode::JSONParseError;
}

template <SerializationPrimitiveConcept T>
std::variant<Json, Error> serialize(T object)
{
  Json json = object;
  return json;
}

template <typename T>
std::variant<Json, Error> serialize(std::pair<std::string, T> const &entry);

template <SerializationPrimitiveConcept T>
std::variant<Json, Error> serialize(std::pair<std::string, T> const &entry)
{
  Json json;
  json[entry.first] = entry.second;
  return json;
}

template <typename Head, typename... Tail>
std::variant<Json, Error> serialize(std::pair<std::string, Head> const &head,
                                    std::pair<std::string, Tail> const &...args)
{
  std::variant<Json, Error> jsonOrError = serialize<Tail...>(args...);

  if (std::holds_alternative<Error>(jsonOrError)) {
    return jsonOrError;
  }

  std::variant<Json, Error> headJsonOrError = serialize<Head>(head);

  if (std::holds_alternative<Error>(headJsonOrError)) {
    return headJsonOrError;
  }

  auto const &[key, value] = head;

  std::get<Json>(jsonOrError)[key] = std::get<Json>(headJsonOrError);
  return jsonOrError;
}

} // namespace PB::Text