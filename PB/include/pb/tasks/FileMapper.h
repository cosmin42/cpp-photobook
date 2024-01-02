#pragma once

#include <filesystem>
#include <set>
#include <vector>

#include <pb/Config.h>
#include <pb/FileComparator.h>
#include <pb/util/Concepts.h>
#include <pb/util/Observable.h>
#include <pb/util/SequentialTaskConsumer.h>
#include <pb/util/Traits.h>

namespace PB {

class MediaMapper final {
public:
  explicit MediaMapper(std::filesystem::path const &root);

  ~MediaMapper() = default;

  void taskStep();
  bool stoppingCondition();

  Path              root() const;
  std::vector<Path> importedDirectories() const;

private:
  std::filesystem::recursive_directory_iterator mRecursiveIterator;
  Path                                          mRoot;
  std::vector<Path>                             mSubFiles;
};

} // namespace PB