#pragma once

#include <variant>

#pragma warning(push)
#pragma warning(disable : 4996)
#include <spdlog/spdlog.h>
#pragma warning(pop)

#include <pb/util/Concepts.h>
#include <pb/util/Error.h>
#include <pb/util/Traits.h>

namespace PB::Text {

template <typename T> std::variant<T, PBDev::Error> deserialize(Json jsonData);

template <typename T>
std::variant<T, PBDev::Error> deserialize(Json jsonData, std::string key,
                                          T    defaultValue = T(),
                                          bool optional = false)
{
  if (jsonData.contains(key)) {
    return deserialize<T>(jsonData.at(key));
  }
  if (optional) {
    return defaultValue;
  }
  return PBDev::Error() << ErrorCode::JSONParseError;
}

template <template <typename> typename Container,
          SerializationPrimitiveConcept T>
std::variant<Container<T>, PBDev::Error> deserialize(Json jsonData)
{
  Container<T> result;
  for (const auto &stagedFolderJson : jsonData) {
    auto deserializedOrError = deserialize<T>(stagedFolderJson);
    if (std::holds_alternative<PBDev::Error>(deserializedOrError)) {
      return std::get<PBDev::Error>(deserializedOrError);
    }
    else {
      result.push_back(std::get<T>(deserializedOrError));
    }
  }
  return result;
}

template <template <typename> typename Container,
          SerializationPrimitiveConcept T>
std::variant<Container<T>, PBDev::Error>
deserialize(Json jsonData, std::string key,
            Container<T> defaultValue = Container<T>(), bool optional = false)
{
  if (jsonData.contains(key)) {
    return deserialize<Container, T>(jsonData.at(key));
  }
  if (optional) {
    return defaultValue;
  }
  return PBDev::Error() << ErrorCode::JSONParseError;
}

std::variant<std::vector<Path>, PBDev::Error> deserializeSpecial(Json jsonData);

std::variant<std::vector<Path>, PBDev::Error>
deserializeSpecial(Json jsonData, std::string key,
                   std::vector<Path> defaultValue = std::vector<Path>(),
                   bool              optional = false);

} // namespace PB::Text