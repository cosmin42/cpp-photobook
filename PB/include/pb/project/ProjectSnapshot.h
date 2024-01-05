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
class ProjectSnapshot {
public:
  static std::shared_ptr<PlatformInfo> platformInfo;
  static Path parentDirectory() { return platformInfo->localStatePath; }

  void               uuid(boost::uuids::uuid newUuid) { mUuid = newUuid; }
  boost::uuids::uuid uuid() const { return mUuid; }

  std::string name() const { return mName; }
  void        name(std::string name) { mName = name; }

  void setImportedPaths(std::vector<Path> paths);
  void setStagedImages(std::vector<std::shared_ptr<VirtualImage>> paths);
  void setStagedImages(std::vector<Path> stagedImages);
  void setPaperSettings(PaperSettings paperSettings);

  std::vector<Path> importedFolderList() const;
  std::vector<Path> stagedImagesList() const;
  PaperSettings     paperSettings() const;

  PathCache &pathCache();

private:
  boost::uuids::uuid mUuid = boost::uuids::random_generator()();

  std::vector<Path> mImportedPaths;
  std::vector<Path> mStagedImages;

  PaperSettings mPaperSettings;

  PathCache mPathCache;

  std::string mName;
};
} // namespace PB