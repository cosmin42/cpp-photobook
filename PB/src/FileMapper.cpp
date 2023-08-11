#include <FileMapper.h>

namespace PB {

FilesMap::FilesMap(const std::string &rootDirectory)
    : mRootDirectory{rootDirectory}
{
}

FilesMap::FilesMap(const std::string &&rootDirectory)
    : mRootDirectory{rootDirectory}
{
}

auto FilesMap::map() const -> EitherFolderOrFile
{
    return EitherFolderOrFile(FileData());
}

} // namespace PB