#pragma once

#include <variant>

#include <boost/functional/hash.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

#include <pb/Config.h>
#include <pb/infra/Traits.h>

namespace PB {
class ProjectMetadata final {
public:
  explicit ProjectMetadata(std::string uuid, std::string path);

  ProjectMetadata() = default;
  ~ProjectMetadata() = default;

  std::pair<boost::uuids::uuid, Path> data() const;

  Path               projectFile() const;
  boost::uuids::uuid uuid() const;

  bool operator==(ProjectMetadata const &other) const;

private:
  boost::uuids::uuid mUUID = boost::uuids::random_generator()();
  Path               mProjectFilePath;
};
} // namespace PB