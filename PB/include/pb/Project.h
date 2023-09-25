#pragma once

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

#include <pb/Config.h>
#include <pb/util/Traits.h>

namespace PB {
struct ProjectDetails {
  boost::uuids::uuid uuid;
  std::string        name;
  Path               parentDirectory;
};

template <typename PersistenceType> class Project final {
public:
  Project()
  {
    mProjectDetails.uuid = boost::uuids::random_generator()();
    mProjectDetails.name = "Untitled";
    mProjectDetails.parentDirectory = PersistenceType::localFolder();
  }
  ~Project() = default;

  ProjectDetails &details() { return mProjectDetails; }

private:
  ProjectDetails mProjectDetails;
};
} // namespace PB