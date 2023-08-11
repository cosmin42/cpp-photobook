#include <FileMapper.h>

#include <optional>
#include <queue>

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

auto FilesMap::map() const -> std::shared_ptr<DataNode>
{
  std::filesystem::path     rootPath(mRootDirectory);
  std::shared_ptr<DataNode> rootNode = std::make_shared<DataNode>();

  auto data = wrap(rootPath);

  if (!data) {
    return nullptr;
  }



  return nullptr;
}

std::optional<EitherFolderOrFile>
FilesMap::wrap(const std::filesystem::path &path)
{
  if (std::filesystem::is_regular_file(path)) {
    return FileData{path.filename().string(), path};
  }
  else if (std::filesystem::is_directory(path)) {
    return FolderData{path.filename().string(), path};
  }
  printInfo("Skipping %s, not a directory or a file.",
            path.filename().string().c_str());
  return std::nullopt;
}

} // namespace PB