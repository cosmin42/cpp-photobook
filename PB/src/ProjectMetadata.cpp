#include <pb/project/ProjectMetadata.h>

namespace PB {

std::pair<boost::uuids::uuid, Path> ProjectMetadata::data() const
{
  return {uuid, projectFilePath};
}

bool ProjectMetadata::operator==(ProjectMetadata const &other) const
{
  return uuid == other.uuid && projectFilePath == other.projectFilePath;
}
} // namespace PB