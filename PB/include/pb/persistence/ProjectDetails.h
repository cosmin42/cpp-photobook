#pragma once
#include <boost/functional/hash.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

#include <pb/Config.h>
#include <pb/persistence/ProjectMetadata.h>
#include <pb/util/Traits.h>

namespace PB {
class ProjectDetails {
public:
  void               uuid(boost::uuids::uuid newUuid) { mUuid = newUuid; }
  boost::uuids::uuid uuid() const { return mUuid; }

  void supportDirName(std::string const dirName) { mSupportDirName = dirName; }
  std::string supportDirName() const { return mSupportDirName; }

  void parentDirectory(Path const dirName) { mParentDirectory = dirName; }
  Path parentDirectory() const { return mParentDirectory; }

  Path supportFolder() const;
  Path projectFile() const;

  operator std::unordered_map<std::string, std::string>() const;
  operator Json() const;

private:
  boost::uuids::uuid mUuid;
  std::string        mSupportDirName;
  Path               mParentDirectory;

  std::vector<Path> mImportedPaths;
  std::vector<Path> mStagedImages;
};
} // namespace PB