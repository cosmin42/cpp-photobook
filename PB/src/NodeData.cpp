#include <NodeData.h>

namespace PB {

NodeData::NodeData(std::string name, std::filesystem::path path)
    : mName{std::move(name)}, mPath{std::move(path)} {}

[[nodiscard]] auto NodeData::name() const -> std::string {
  return mName;
}

[[nodiscard]] auto NodeData::path() const -> std::filesystem::path {
  return mPath;
}

void NodeData::setName(std::string &newName) {
  mName = newName;
}

void NodeData::setName(std::string &&newName) {
  mName = newName;
}

void NodeData::setPath(std::filesystem::path &newPath) {
  mPath = newPath;
}

void NodeData::setPath(std::filesystem::path &&newPath) {
  mPath = newPath;
}

} // namespace PB