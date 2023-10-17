#pragma once
#include <boost/functional/hash.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

#include <pb/Config.h>
#include <pb/persistence/ProjectMetadata.h>
#include <pb/util/Traits.h>

namespace PB {
struct ProjectDetails {
  boost::uuids::uuid uuid;
  std::string        supportDirName;
  Path               parentDirectory;

  std::vector<Path> importedPaths;
  std::vector<Path> stagedImages;

  Path supportFolder() const;
  Path projectFile() const;

  operator std::unordered_map<std::string, std::string>() const;

  operator Json() const;
};
} // namespace PB