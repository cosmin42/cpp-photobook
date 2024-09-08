#pragma once

#include <variant>

#include <pb/util/Util.h>

namespace PB {
/*
class FilePersistence final {
public:
  FilePersistence() = default;
  explicit FilePersistence(Path path);
  ~FilePersistence() = default;

  void read(std::function<void(std::variant<Json, PBDev::Error>)> onReturn);

  void write(Json                                             jsonData,
             std::function<void(std::optional<PBDev::Error>)> onReturn);

private:
  std::variant<std::unordered_map<std::string, std::string>, PBDev::Error>
  parseData(std::string const &rawData);

  Path mPath;
};
*/
} // namespace PB