#pragma once

#include <boost/functional/hash.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

#include <pb/Config.h>
#include <pb/project/ProjectSnapshot.h>
#include <pb/util/Util.h>

namespace PB {

class Project final {
public:
  Project()
  {
    // TODO: Check if it is not redundant
    auto uuid = boost::uuids::random_generator()();
    auto projectPath =
        ProjectSnapshot::parentDirectory() /
        (boost::uuids::to_string(uuid) + Context::BOOK_EXTENSION);

    mMetadata =
        ProjectMetadata(boost::uuids::to_string(uuid), projectPath.string());

    mCache.uuid = uuid;
    mCache.paperSettings = Context::A4_LANDSCAPE_PAPER;

    mActive = mCache;
  }

  Project(ProjectSnapshot projectDetails)
  {
    auto projectPath = ProjectSnapshot::parentDirectory() /
                       (boost::uuids::to_string(projectDetails.uuid) +
                        Context::BOOK_EXTENSION);
    mMetadata = ProjectMetadata(boost::uuids::to_string(projectDetails.uuid),
                                projectPath.string());

    mCache = projectDetails;
    mActive = mCache;
  }

  ~Project() = default;

  ProjectMetadata const &metadata() { return mMetadata; }
  ProjectSnapshot       &active() { return mActive; }
  ProjectSnapshot const &cache() { return mCache; }

  void save() { mCache = mActive; }

private:
  ProjectMetadata mMetadata;

  ProjectSnapshot mCache;

  ProjectSnapshot mActive;
};

} // namespace PB