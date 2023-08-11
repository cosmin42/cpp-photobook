#include <NodeData.h>

namespace PB {

NodeData::NodeData(std::string name, std::filesystem::path path)
    : mUUID{boost::uuids::random_generator()()}, mName{std::move(name)},
      mPath{std::move(path)}
{
}

NodeData::NodeData(const NodeData &nodeData)
    : mUUID{nodeData.mUUID}, mName{nodeData.mName}, mPath{nodeData.mPath}
{
}

NodeData::NodeData(const NodeData &&nodeData)
    : mUUID{nodeData.mUUID}, mName{nodeData.mName}, mPath{nodeData.mPath}
{
}

NodeData &NodeData::operator=(const NodeData &nodeData)
{
  mUUID = nodeData.mUUID;
  mName = nodeData.mName;
  mPath = nodeData.mPath;
  return *this;
}

NodeData &NodeData::operator=(const NodeData &&nodeData)
{
  mUUID = nodeData.mUUID;
  mName = nodeData.mName;
  mPath = nodeData.mPath;
  return *this;
}

[[nodiscard]] auto NodeData::name() const -> std::string { return mName; }

[[nodiscard]] auto NodeData::path() const -> std::filesystem::path
{
  return mPath;
}

void NodeData::setName(std::string &newName) { mName = newName; }

void NodeData::setName(std::string &&newName) { mName = newName; }

void NodeData::setPath(std::filesystem::path &newPath) { mPath = newPath; }

void NodeData::setPath(std::filesystem::path &&newPath) { mPath = newPath; }

} // namespace PB