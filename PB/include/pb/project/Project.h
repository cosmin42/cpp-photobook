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
  static std::string generateAlbumName(std::function<bool(std::string)>);

  PaperSettings paperSettings = OneConfig::A4_LANDSCAPE_PAPER;
  std::string   name;

  ~Project() = default;

  ImageMonitor &imageMonitor() { return mImageMonitor; }
  StagedImages &stagedImages() { return mStagedImages; }

private:
  static std::vector<std::string> HAPPY_WORDS;

  ImageMonitor mImageMonitor;
  StagedImages mStagedImages;
};

// TODO: Do a generic Identifyable class
typedef std::pair<boost::uuids::uuid, Project> IdentifyableProject;

} // namespace PB