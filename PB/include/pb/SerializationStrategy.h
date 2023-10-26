#pragma once

#include <pb/persistence/Persistence.h>

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

} // namespace PB