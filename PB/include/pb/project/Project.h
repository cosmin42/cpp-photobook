#pragma once

#include <boost/functional/hash.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

#include <pb/Config.h>
#include <pb/Platform.h>
#include <pb/util/Util.h>

namespace PB {

class Project final {
public:
  static std::string generateAlbumName(std::function<bool(std::string)>);

  PaperSettings paperSettings = OneConfig::A4_LANDSCAPE_PAPER;

  std::string name;

  ~Project() = default;

private:
  static std::vector<std::string> HAPPY_WORDS;
};

} // namespace PB