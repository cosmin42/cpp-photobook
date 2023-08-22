#include <FileMapper.h>

#include <queue>

#include <ConfigDependants.h>

namespace PB {

FilesMap::FilesMap(const std::string &rootDirectory)
    : mRootDirectory{rootDirectory}
{
  printDebug("FilesMap ctr %s\n", rootDirectory.c_str());
}

FilesMap::FilesMap(std::string &&rootDirectory) : mRootDirectory{rootDirectory}
{
  printDebug("FilesMap ctr %s\n", rootDirectory.c_str());
}

auto FilesMap::map() const -> std::shared_ptr<DataNode>
{
  assert(std::filesystem::exists(mRootDirectory) &&
         "The root folder is missing");

  std::filesystem::path rootPath(mRootDirectory);

  auto [dataId, data] = wrap(rootPath);

  if (!data) {
    return nullptr;
  }

  std::shared_ptr<DataNode> rootNode = std::make_shared<DataNode>(data.value());

  std::queue<std::shared_ptr<DataNode>> nodesQueue;
  nodesQueue.push(rootNode);

  while (!nodesQueue.empty()) {
    auto currentNode = nodesQueue.front();
    nodesQueue.pop();

    std::visit(
        overloaded{
            [](FileData &) {},
            [&nodesQueue, currentNode{currentNode}](FolderData &folderData) {
              for (const auto &entry :
                   std::filesystem::directory_iterator(folderData.path())) {
                auto [dataId, data] = wrap(entry);
                if (data) {
                  auto dataRef = std::make_shared<DataNode>(data.value());
                  currentNode->children[dataId] = dataRef;
                  nodesQueue.push(dataRef);
                }
              }
            }},
        currentNode->data);
  }

  return rootNode;
}

std::pair<boost::uuids::uuid, std::optional<DataNode>>
FilesMap::wrap(const std::filesystem::path &path)
{
  if (std::filesystem::is_regular_file(path)) {
    FileData fileData{path.filename().string(), path};
    return {
        fileData.id(),
        DataNode{fileData,
                 std::map<boost::uuids::uuid, std::shared_ptr<DataNode>>()}};
  }
  else if (std::filesystem::is_directory(path)) {
    FolderData folderData{path.filename().string(), path};
    return {
        folderData.id(),
        DataNode{folderData,
                 std::map<boost::uuids::uuid, std::shared_ptr<DataNode>>()}};
  }
  printInfo("Skipping %s, not a directory or a file.\n",
            path.filename().string().c_str());
  return {boost::uuids::uuid(), std::nullopt};
}

} // namespace PB