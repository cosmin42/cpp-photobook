#pragma once

#include <filesystem>
#include <set>
#include <vector>

#include <pb/Config.h>
#include <pb/FileComparator.h>
#include <pb/tasks/SequentialTaskConsumer.h>
#include <pb/util/Util.h>

// #define SIMULATE_SLOW_MAPPER

namespace PB {

class MediaMapper final {
public:
  explicit MediaMapper(std::filesystem::path const &root);

  ~MediaMapper() = default;

  void taskStep();
  int  stepsCount() const;
  bool stoppingCondition() const;

  Path              root() const;
  std::vector<Path> importedDirectories() const;

private:
  std::filesystem::recursive_directory_iterator mRecursiveIterator;
  Path                                          mRoot;
  std::vector<Path>                             mSubFiles;
};

} // namespace PB