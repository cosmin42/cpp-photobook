#pragma once

#include <filesystem>
#include <optional>
#include <source_location>
#include <utility>

#include <nlohmann/json.hpp>

#include <boost/functional/hash.hpp>

#include <pb/RuntimeUUID.h>

namespace PBDev {

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
typedef nlohmann::json        Json;

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

void basicAssert(int shouldBetrue, const std::source_location location =
                                       std::source_location::current());

template <typename Tag> class StrongUuid final {
public:
  explicit StrongUuid(const boost::uuids::uuid &uuid) : mUuid(uuid) {}
  ~StrongUuid() = default;

  const boost::uuids::uuid &operator*() const { return mUuid; }

  bool operator==(const StrongUuid<Tag> &other) const
  {
    return mUuid == other.mUuid;
  }

  bool operator!=(const StrongUuid<Tag> &other) const
  {
    return !(*this == other);
  }
  // TODO:this is redundant with the * operator above
  boost::uuids::uuid raw() const { return mUuid; }

private:
  boost::uuids::uuid mUuid;
};

template <typename Tag> class StrongString final {
public:
  explicit StrongString(const std::string &key) : mKey(key) {}
  ~StrongString() = default;

  const std::string &operator*() const { return mKey; }

  bool operator==(const StrongString<Tag> &other) const
  {
    return mKey == other.mKey;
  }

  bool operator!=(const StrongString<Tag> &other) const
  {
    return !(*this == other);
  }

private:
  std::string mKey;
};

} // namespace PBDev

#define DECLARE_STRONG_UUID(NAME)                                              \
  namespace PBDev {                                                            \
  struct NAME##Tag {};                                                         \
  typedef StrongUuid<NAME##Tag> NAME;                                          \
  }                                                                            \
  namespace boost {                                                            \
  template <> struct hash<::PBDev::NAME> {                                     \
    ::std::size_t operator()(const ::PBDev::NAME &suuid) const                 \
    {                                                                          \
      return ::boost::hash<boost::uuids::uuid>()(*suuid);                      \
    }                                                                          \
  };                                                                           \
  }

#define DECLARE_STRONG_STRING(NAME)                                            \
  namespace PBDev {                                                            \
  struct NAME##Tag {};                                                         \
  typedef StrongString<NAME##Tag> NAME;                                        \
  }                                                                            \
  namespace boost {                                                            \
  template <> struct hash<::PBDev::NAME> {                                     \
    ::std::size_t operator()(const ::PBDev::NAME &str) const                   \
    {                                                                          \
      return ::boost::hash<std::string>()(*str);                               \
    }                                                                          \
  };                                                                           \
  }

DECLARE_STRONG_UUID(ParallelTaskConsumerId)
DECLARE_STRONG_UUID(PdfPoDoFoId)

    typedef PBDev::Path Path;
typedef PBDev::Json Json;

#define UNUSED(x)                                                              \
  do {                                                                         \
    (void)x;                                                                   \
  } while (0);
