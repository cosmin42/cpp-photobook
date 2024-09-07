#pragma once

#include <boost/functional/hash.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

#include <pb/Config.h>
#include <pb/Platform.h>
#include <pb/util/Util.h>

namespace PB {

struct Project {
  PaperSettings paperSettings = OneConfig::A4_LANDSCAPE_PAPER;
};

} // namespace PB