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

auto FilesMap::map() const -> TreeNodeDataWrapper
{
    return TreeNodeDataWrapper();
}

} // namespace PB