#include <NodeData.h>

namespace PB {

NodeData::NodeData(std::string name, std::filesystem::path path)
    : mName{name}, mPath{path} {}

[[nodiscard]] auto NodeData::name() const noexcept -> std::string {
  return mName;
}

[[nodiscard]] auto NodeData::path() const noexcept -> std::filesystem::path {
  return mPath;
}

void NodeData::setName(std::string &newName) noexcept {
  mName = newName;
}

void NodeData::setName(std::string &&newName) noexcept {
  mName = newName;
}

void NodeData::setPath(std::filesystem::path &newPath) noexcept {
  mPath = newPath;
}

void NodeData::setPath(std::filesystem::path &&newPath) noexcept {
  mPath = newPath;
}

} // namespace PB