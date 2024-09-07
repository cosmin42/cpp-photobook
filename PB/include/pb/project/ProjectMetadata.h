#pragma once

#include <variant>

#include <boost/functional/hash.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

#include <pb/Config.h>
#include <pb/util/Util.h>

namespace PB {

struct ProjectMetadata {
  Path               projectFilePath;
  boost::uuids::uuid uuid;
  std::string        name;

  std::pair<boost::uuids::uuid, Path> data() const;

  bool operator==(ProjectMetadata const &other) const;
};

} // namespace PB