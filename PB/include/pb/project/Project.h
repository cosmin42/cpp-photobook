#pragma once

#include <boost/functional/hash.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

#include <pb/Config.h>
#include <pb/Platform.h>
#include <pb/project/ProjectSnapshot.h>
#include <pb/util/Util.h>

// #define SIMULATE_FEW_HAPPY_WORDS

namespace PB {

// TODO: Merge Project with ProjectSnapshot.
class Project final {
public:
  static std::string generateAlbumName(std::function<bool(std::string)>);
  static std::shared_ptr<PlatformInfo> platformInfo;
  static Path parentDirectory() { return platformInfo->localStatePath; }

  PaperSettings paperSettings = Context::A4_LANDSCAPE_PAPER;

  ~Project() = default;

private:
  static std::vector<std::string> HAPPY_WORDS;
};

} // namespace PB