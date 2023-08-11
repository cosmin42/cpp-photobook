#pragma once

#include <NodeData.h>
#include <unordered_map>

#include <boost/uuid/uuid_generators.hpp>

namespace PB {
struct TreeNodeDataWrapper {
  NodeData                                         data;
  std::unordered_map<boost::uuids::uuid, NodeData> children;
};
} // namespace PB