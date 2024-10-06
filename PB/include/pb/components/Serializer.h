#pragma once

#include <variant>

#pragma warning(push)
#pragma warning(disable : 4996)
#include <spdlog/spdlog.h>
#pragma warning(pop)

#include <pb/util/Concepts.h>
#include <pb/util/Traits.h>

namespace PB {
// TODO: There is still some work to do here to handle the & const and && cases.

template <SerializationPrimitiveConcept T>
std::variant<Json, PBDev::Error> flatSimple(int depth, T object)
{
  UNUSED(depth);
#ifdef _CLANG_UML_
  Json json;
#else
  Json json = object;
#endif
  return json;
}

template <typename T>
std::variant<Json, PBDev::Error> flatSimple(int depth, T entry);

template <SerializationPrimitiveConcept T>
std::variant<Json, PBDev::Error> flatAndTagSimple(int depth, std::string tag,
                                                  T object)
{
  Json json;
  json[tag] = object;
#ifndef _CLANG_UML_
  spdlog::info("{}T {}\n", std::string(depth * 2, ' '), json.dump());
#endif
  return json;
}

template <typename T>
std::variant<Json, PBDev::Error> flatAndTagSimple(int depth, std::string tag,
                                                  T object)
{
  Json json;

  std::variant<Json, PBDev::Error> jsonOrError = flatSimple(depth, object);

  if (std::holds_alternative<PBDev::Error>(jsonOrError)) {
    return jsonOrError;
  }
  json[tag] = std::get<Json>(jsonOrError);
#ifndef _CLANG_UML_
  spdlog::info("{}T {}\n", std::string(depth * 2, ' '), json.dump());
#endif
  return json;
}

template <typename Head, typename... Tail>
std::variant<Json, PBDev::Error>
flatDictionary(int depth, std::tuple<std::string, Head> const &head)
{
  Json json;
  auto &[tag, object] = head;

  std::variant<Json, PBDev::Error> headJsonOrError =
      flatMaybeContainer<Head>(depth + 1, object);

  json[tag] = std::get<Json>(headJsonOrError);

  return json;
}

template <typename Head, typename... Tail>
std::variant<Json, PBDev::Error>
flatDictionary(int depth, std::tuple<std::string, Head> const &head,
               std::tuple<std::string, Tail> const &...args)
{
  std::variant<Json, PBDev::Error> jsonOrError =
      flatDictionary<Tail...>(depth + 1, args...);

  if (std::holds_alternative<PBDev::Error>(jsonOrError)) {
    return jsonOrError;
  }
#ifndef _CLANG_UML_
  spdlog::info("{}T {}\n", std::string(depth * 2, ' '),
               std::get<Json>(jsonOrError).dump());
#endif
  auto &[tag, object] = head;

  std::variant<Json, PBDev::Error> headJsonOrError =
      flatMaybeContainer<Head>(depth + 1, object);

  if (std::holds_alternative<PBDev::Error>(headJsonOrError)) {
    return headJsonOrError;
  }

  Json taggedJson;
  taggedJson[tag] = std::get<Json>(headJsonOrError);

#ifndef _CLANG_UML_
  spdlog::info("{}T {}\n", std::string(depth * 2, ' '), taggedJson.dump());
#endif
  std::get<Json>(jsonOrError).update(taggedJson);
#ifndef _CLANG_UML_
  spdlog::info("{}T {}\n", std::string(depth * 2, ' '),
               std::get<Json>(jsonOrError).dump());
#endif
  return jsonOrError;
}

template <typename... Tail>
std::variant<Json, PBDev::Error>
flatAndTagDictionary(int depth, std::string tag,
                     std::tuple<std::string, Tail> const &...args)
{
  std::variant<Json, PBDev::Error> jsonOrError =
      flatDictionary<Tail...>(depth + 1, args...);

  if (std::holds_alternative<PBDev::Error>(jsonOrError)) {
    return jsonOrError;
  }

  Json json;
  json[tag] = std::get<Json>(jsonOrError);
  return json;
}
template <typename T>
std::variant<Json, PBDev::Error> flatMaybeContainer(int      depth,
                                                    T const &maybeContainer)
{
  if constexpr (ContainerConcept<T>) {
    Json json;
    for (auto element : maybeContainer) {
      using ValueType = typename T::value_type;
      std::variant<Json, PBDev::Error> elementOrError =
          flatMaybeContainer<ValueType>(depth + 1, element);
      if (std::holds_alternative<PBDev::Error>(elementOrError)) {
        return elementOrError;
      }
      json.push_back(std::get<Json>(elementOrError));
    }
    return json;
  }
  else if constexpr (StdMapsConcept<T>) {
    Json json;
    for (auto [key, value] : maybeContainer) {
      std::variant<Json, PBDev::Error> valueOrError =
          flatMaybeContainer(depth + 1, value);
      if (std::holds_alternative<PBDev::Error>(valueOrError)) {
        return valueOrError;
      }

      json[key] = std::get<Json>(valueOrError);
    }

    return json;
  }
  else if constexpr (BimapConcept<T>) {
    Json json;
    for (auto entry : maybeContainer) {
      auto                            &value = entry.right;
      std::variant<Json, PBDev::Error> valueOrError =
          flatMaybeContainer(depth + 1, value);
      if (std::holds_alternative<PBDev::Error>(valueOrError)) {
        return valueOrError;
      }

      json[entry.left] = std::get<Json>(valueOrError);
    }
    return json;
  }
  else {
    return flatSimple<T>(depth, maybeContainer);
  }
}

} // namespace PB
