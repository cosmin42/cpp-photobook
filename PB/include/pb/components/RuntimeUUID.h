#pragma once

#include <cstdint>

#include <boost/uuid/uuid.hpp>

namespace PB {

class RuntimeUUID final {
public:
  static boost::uuids::uuid newUUID();

private:
  static constexpr size_t UUID_SIZE = 16;
  static uint8_t          guid[UUID_SIZE];

  RuntimeUUID() = delete;
  ~RuntimeUUID() = delete;

  static void increment();
};

} // namespace PB