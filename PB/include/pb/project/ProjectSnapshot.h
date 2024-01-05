#pragma once
#include <boost/functional/hash.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

#include <pb/Config.h>
#include <pb/image/Image.h>
#include <pb/persistence/SerializationStrategy.h>
#include <pb/project/PaperSettings.h>
#include <pb/project/PathCache.h>
#include <pb/project/ProjectMetadata.h>
#include <pb/util/Util.h>

namespace PB {
struct ProjectSnapshot {
  static std::shared_ptr<PlatformInfo> platformInfo;
  static Path parentDirectory() { return platformInfo->localStatePath; }

  boost::uuids::uuid uuid = boost::uuids::random_generator()();
  std::vector<Path>  importedPaths;
  std::vector<Path>  stagedImages;
  PaperSettings      paperSettings;
  PathCache          pathCache;
  std::string        name = boost::uuids::to_string(uuid);
};

} // namespace PB