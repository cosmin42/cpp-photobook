#include <FileData.h>

namespace PB {

FileData::FileData(std::string name, std::filesystem::path path)
    : mUUID{boost::uuids::random_generator()()}, mName{std::move(name)},
      mPath{std::move(path)}
{
}

FileData::FileData(const FileData &fileData)
    : mUUID{fileData.mUUID}, mName{fileData.mName}, mPath{fileData.mPath}
{
}

FileData::FileData(const FileData &&fileData)
    : mUUID{fileData.mUUID}, mName{fileData.mName}, mPath{fileData.mPath}
{
}

FileData &FileData::operator=(const FileData &fileData)
{
  mUUID = fileData.mUUID;
  mName = fileData.mName;
  mPath = fileData.mPath;
  return *this;
}

FileData &FileData::operator=(const FileData &&fileData)
{
  mUUID = fileData.mUUID;
  mName = fileData.mName;
  mPath = fileData.mPath;
  return *this;
}

[[nodiscard]] auto FileData::name() const -> std::string { return mName; }

[[nodiscard]] auto FileData::path() const -> std::filesystem::path
{
  return mPath;
}

[[nodiscard]] auto FileData::id() const -> boost::uuids::uuid
{
  return mUUID;
}

void FileData::setName(std::string &newName) { mName = newName; }

void FileData::setName(std::string &&newName) { mName = newName; }

void FileData::setPath(std::filesystem::path &newPath) { mPath = newPath; }

void FileData::setPath(std::filesystem::path &&newPath) { mPath = newPath; }

} // namespace PB