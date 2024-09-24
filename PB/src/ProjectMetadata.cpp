#include <pb/entities/ProjectMetadata.h>

namespace PB {
ProjectMetadata::ProjectMetadata(std::string uuid, std::string path)
{
  try {
    boost::uuids::string_generator gen;
    mUUID = gen(uuid);
  }
  catch (...) {
    PBDev::basicAssert(false);
  }
  mProjectFilePath = path;
}

std::pair<boost::uuids::uuid, Path> ProjectMetadata::data() const
{
  return {mUUID, mProjectFilePath};
}

Path ProjectMetadata::projectFile() const { return mProjectFilePath; }

boost::uuids::uuid ProjectMetadata::uuid() const { return mUUID; }

bool ProjectMetadata::operator==(ProjectMetadata const &other) const
{
  return mUUID == other.mUUID && mProjectFilePath == other.mProjectFilePath;
}
} // namespace PB