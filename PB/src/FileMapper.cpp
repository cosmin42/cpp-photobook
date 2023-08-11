#include <FileMapper.h>

#include <ConfigDependants.h>

namespace PB {

FilesMap::FilesMap(const std::string &rootDirectory)
    : mRootDirectory{rootDirectory}
{
  printDebug("FilesMap ctr %s\n", rootDirectory.c_str());
}

FilesMap::FilesMap(const std::string &&rootDirectory)
    : mRootDirectory{rootDirectory}
{
  printDebug("FilesMap ctr %s\n", rootDirectory.c_str());
}

auto FilesMap::map() const -> EitherFolderOrFile
{
  return EitherFolderOrFile(FileData());
}

} // namespace PB