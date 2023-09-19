#pragma once
#include <unordered_map>

namespace PB {
template <typename T> class Persistence final {
public:
  Persistence() = default;
  Persistence(Persistence const &) = delete;
  Persistence(Persistence &&) noexcept = delete;
  Persistence &operator=(Persistence const &) = delete;
  ~Persistence() = default;

  void write() {}
  void load() {}

  std::unordered_map<std::string, std::string> &cache() { return mCache; }

private:
  std::unordered_map<std::string, std::string> mCache;
  T                                            mPersistence;
};

} // namespace PB