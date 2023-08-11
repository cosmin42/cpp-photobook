#include <FolderData.h>

namespace PB {

FolderData::FolderData(std::string name, std::filesystem::path path)
    : mUUID{boost::uuids::random_generator()()}, mName{std::move(name)},
      mPath{std::move(path)}
{
}

FolderData::FolderData(const FolderData &folderData)
    : mUUID{folderData.mUUID}, mName{folderData.mName}, mPath{folderData.mPath}
{
}

FolderData::FolderData(const FolderData &&folderData)
    : mUUID{folderData.mUUID}, mName{folderData.mName}, mPath{folderData.mPath}
{
}

FolderData &FolderData::operator=(const FolderData &folderData)
{
  mUUID = folderData.mUUID;
  mName = folderData.mName;
  mPath = folderData.mPath;
  return *this;
}

FolderData &FolderData::operator=(const FolderData &&folderData)
{
  mUUID = folderData.mUUID;
  mName = folderData.mName;
  mPath = folderData.mPath;
  return *this;
}

[[nodiscard]] auto FolderData::name() const -> std::string { return mName; }

[[nodiscard]] auto FolderData::path() const -> std::filesystem::path
{
  return mPath;
}

void FolderData::setName(std::string &newName) { mName = newName; }

void FolderData::setName(std::string &&newName) { mName = newName; }

void FolderData::setPath(std::filesystem::path &newPath) { mPath = newPath; }

void FolderData::setPath(std::filesystem::path &&newPath) { mPath = newPath; }

} // namespace PB