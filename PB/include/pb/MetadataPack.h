#pragma once

#include <string>
#include <unordered_set>

#include <boost/bimap/bimap.hpp>
#include <boost/uuid/uuid.hpp>

#include <pb/util/Traits.h>

namespace PB {
struct MetadataPack {
  boost::bimaps::bimap<boost::uuids::uuid, std::string> metadata;
  boost::bimaps::bimap<Path, std::string>               pathCache;
  std::unordered_set<std::pair<Path, std::string>, PathProjectAssociationHash>
      pathProject;
};
} // namespace PB