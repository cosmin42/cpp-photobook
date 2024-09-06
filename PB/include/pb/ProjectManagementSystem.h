#pragma once

#include <boost/bimap/bimap.hpp>
#include <boost/uuid/uuid.hpp>

#include <pb/util/Traits.h>

namespace PB {

class ProjectManagementSystemListener {};

class ProjectManagementSystem final {
public:
  ~ProjectManagementSystem() = default;

  std::string newAlbumName();

private:
  static std::vector<std::string> HAPPY_WORDS;

  boost::bimaps::bimap<boost::uuids::uuid, std::string> mProjectsMetadata;

  bool hasProjectName(std::string name) const;
};
} // namespace PB
