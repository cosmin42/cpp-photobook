#pragma once

#include <variant>

#include <pb/Error.h>
#include <pb/util/Traits.h>

namespace PB {

class FilePersistence final {
public:
  static bool createDirectory(Path path);

  explicit FilePersistence(Path path);

  void read(std::function<void(std::variant<Json, Error>)> onReturn);

  void write(Json jsonData, std::function<void(std::optional<Error>)> onReturn);

private:
  std::variant<std::unordered_map<std::string, std::string>, Error>
  parseData(std::string const &rawData);

  Path mPath;
};
} // namespace PB