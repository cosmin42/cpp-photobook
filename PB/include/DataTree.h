#pragma once

#include <NodeData.h>
#include <map>

#include <boost/uuid/uuid_generators.hpp>

namespace PB {
struct TreeNodeDataWrapper {
  NodeData                                         data;
  std::map<boost::uuids::uuid, NodeData> children;
};
} // namespace PB