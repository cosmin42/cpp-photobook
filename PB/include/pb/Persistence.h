#pragma once

#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>

#include <pb/Error.h>

namespace PB {

template <typename PersistenceType> class Persistence final {
public:
  Persistence() = default;
  Persistence(Persistence const &) = delete;
  Persistence(Persistence &&) noexcept = delete;
  Persistence &operator=(Persistence const &) = delete;
  ~Persistence() = default;

  void write(std::function<void(std::optional<Error>)> f)
  {
    mPersistence.setObserver([f{f}](std::optional<Error> out) {
      f(out);
    });
    mPersistence.template write<std::unordered_map>(mCache);
  }

  void load(std::function<void(std::optional<Error>)> f)
  {
    mPersistence.setObserver([f{f}, this](std::optional<Error> out) {
      if (!out) {
        mCache.clear();
        auto &cacheRef = mPersistence.data();
        mCache.insert(cacheRef.begin(), cacheRef.end());
        cacheRef.clear();
      }
      f(out);
    });

    mPersistence.load();
  }

  std::unordered_map<std::string, std::string> &cache() { return mCache; }

private:
  std::unordered_map<std::string, std::string> mCache;
  PersistenceType                              mPersistence;
};

} // namespace PB