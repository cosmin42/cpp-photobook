#pragma once

#include <pb/common/Log.h>
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
std::variant<Json, Error> serialize(int depth, T object)
{
  Json json = object;
  PB::printDebug("%sT %s\n", std::string(depth * 2, ' ').c_str(),
                 json.dump().c_str());
  return json;
}

template <typename T>
std::variant<Json, Error> serialize(int                              depth,
                                    std::pair<std::string, T> const &entry);

template <SerializationPrimitiveConcept T>
std::variant<Json, Error> serialize(int                              depth,
                                    std::pair<std::string, T> const &entry)
{
  Json json;
  json[entry.first] = entry.second;
  PB::printDebug("%s(string, T) %s\n", std::string(depth * 2, ' ').c_str(),
                 json.dump().c_str());
  return json;
}

template <typename Head, typename... Tail>
std::variant<Json, Error> serialize(int                                 depth,
                                    std::pair<std::string, Head> const &head,
                                    std::pair<std::string, Tail> const &...args)
{
  std::variant<Json, Error> jsonOrError =
      serialize<Tail...>(depth + 1, args...);

  if (std::holds_alternative<Error>(jsonOrError)) {
    return jsonOrError;
  }

  PB::printDebug("%s(args...Tail) %s\n", std::string(depth * 2, ' ').c_str(),
                 std::get<Json>(jsonOrError).dump().c_str());

  std::variant<Json, Error> headJsonOrError = serialize<Head>(depth + 1, head);

  if (std::holds_alternative<Error>(headJsonOrError)) {
    return headJsonOrError;
  }

  PB::printDebug("%s(args...Head) %s\n", std::string(depth * 2, ' ').c_str(),
                 std::get<Json>(headJsonOrError).dump().c_str());

  auto const &[key, value] = head;
  std::get<Json>(jsonOrError).update(std::get<Json>(headJsonOrError));

  PB::printDebug("%s(args...All) %s\n", std::string(depth * 2, ' ').c_str(),
                 std::get<Json>(jsonOrError).dump().c_str());
  return jsonOrError;
}

} // namespace PB::Text