#pragma once
#include <boost/functional/hash.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

#include <pb/Config.h>
#include <pb/DataManager.h>
#include <pb/PaperSettings.h>
#include <pb/SerializationStrategy.h>
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

  void setImportedPaths(std::vector<Path> maybePath);
  void setStagedImages(std::vector<std::shared_ptr<VirtualImage>> maybePath);
  void setStagedImages(std::vector<Path> stagedImages);
  void setPaperSettings(PaperSettings paperSettings);
  void removeStagedImage(int index);

  std::vector<Path> importedFolderList() const;
  std::vector<Path> stagedImagesList() const;
  PaperSettings     paperSettings() const;

private:
  boost::uuids::uuid mUuid = boost::uuids::random_generator()();
  std::string        mSupportDirName;
  Path               mParentDirectory;

  std::vector<Path> mImportedPaths;
  std::vector<Path> mStagedImages;

  PaperSettings mPaperSettings;
};
} // namespace PB