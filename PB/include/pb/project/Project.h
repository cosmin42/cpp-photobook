#pragma once

#include <boost/functional/hash.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

#include <pb/Config.h>
#include <pb/project/ProjectSnapshot.h>
#include <pb/util/Util.h>

#define SIMULATE_FEW_HAPPY_WORDS

namespace PB {

class Project final {
public:
  static std::string generateAlbumName(std::function<bool(std::string)>);

  Project();

  explicit Project(std::string name);
  explicit Project(ProjectSnapshot snapshot);

  ~Project() = default;

  ProjectMetadata const &metadata() { return mMetadata; }
  ProjectSnapshot       &active() { return mActive; }
  ProjectSnapshot const &cache() { return mCache; }

  void sync() { mCache = mActive; }

  bool isSynced();

private:
  static std::vector<std::string> HAPPY_WORDS;

  ProjectMetadata mMetadata;

  ProjectSnapshot mCache;

  ProjectSnapshot mActive;
};

} // namespace PB