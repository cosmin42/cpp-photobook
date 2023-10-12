#pragma once

#include <filesystem>
#include <optional>
#include <utility>

namespace PB {

template <class... Ts> struct overloaded : Ts... {
  using Ts::operator()...;
};
template <class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

template <typename Fn> auto compose(Fn &&f)
{
  return [f = std::forward<Fn>(f)](auto &&...args) {
    return f(std::move(args)...);
  };
}

template <typename HeadFn, typename... TailFn>
auto compose(HeadFn &&f, TailFn &&...tailF)
{
  return [f = std::forward<HeadFn>(f),
          ... tailF = (std::forward<TailFn>(tailF))](auto &&...args) {
    return f(compose(tailF...)(args...));
  };
}

typedef std::filesystem::path Path;

template <typename T> void Unused(T &&) {}

template <template <typename...> typename M, typename KeyType,
          typename ValueType, typename...>
std::optional<ValueType> mapGet(M<KeyType, ValueType> const &map,
                                KeyType                    &&value)
{
  if (map.find(value) == map.end()) {
    return std::nullopt;
  }
  return map.at(value);
}

void basicAssert(int shouldBetrue);

} // namespace PB