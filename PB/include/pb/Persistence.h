#pragma once

#include <memory>
#include <string>
#include <unordered_map>

#include <pb/Error.h>
#include <pb/StorageListener.h>

namespace PB {

template <typename TaskManageableType, typename T>
  requires TaskManageableConcept<TaskManageableType>
class Persistence final {
public:
  Persistence() = default;
  Persistence(Persistence const &) = delete;
  Persistence(Persistence &&) noexcept = delete;
  Persistence &operator=(Persistence const &) = delete;
  ~Persistence() = default;

  void addListener(StorageListener<TaskManageableType, T> &listener)
  {
    mPersistence.setObserver([&listener](std::optional<Error> out) {
      if (out) {
        listener.onError();
      }
    });
  }

  void write() { mPersistence.write<std::unordered_map>(mCache); }
  void load() { mPersistence.load(); }

  std::unordered_map<std::string, std::string> &cache() { return mCache; }

private:
  std::unordered_map<std::string, std::string> mCache;
  T                                            mPersistence;
};

} // namespace PB