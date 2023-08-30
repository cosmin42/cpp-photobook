#pragma once

#include <filesystem>
#include <string>
#include <vector>

namespace PB {

class FilesMap final {
public:
  FilesMap() = delete;
  explicit FilesMap(const std::string &rootDirectory);
  FilesMap(FilesMap const &);
  FilesMap(FilesMap &&) = delete;
  FilesMap &operator=(FilesMap const &);

  ~FilesMap() = default;

  auto map() const -> std::vector<std::filesystem::path>;

  void setRootDirectory(std::string const &newRootDir);

private:
  std::string mRootDirectory;

};

} // namespace PB