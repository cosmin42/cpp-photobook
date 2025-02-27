#pragma once

#include <boost/functional/hash.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

#include <pb/Config.h>
#include <pb/ImageMonitor.h>
#include <pb/Platform.h>
#include <pb/StagedImages.h>

namespace PB {

class Project final {
public:
  PaperSettings paperSettings = OneConfig::A4_LANDSCAPE_PAPER;
  std::string   name;

  Project():mImageMonitor(std::make_shared<ImageMonitor>()),
    mStagedImages(std::make_shared<StagedImages>())
  {}
  ~Project() = default;

  std::shared_ptr<ImageMonitor> imageMonitor() { return mImageMonitor; }
  std::shared_ptr<StagedImages> stagedImages() { return mStagedImages; }

private:
  std::shared_ptr<ImageMonitor> mImageMonitor = nullptr;
  std::shared_ptr<StagedImages> mStagedImages = nullptr;
};

// TODO: Do a generic Identifyable class
typedef std::pair<boost::uuids::uuid, Project> IdentifyableProject;

} // namespace PB