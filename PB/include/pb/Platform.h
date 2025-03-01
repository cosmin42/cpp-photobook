#pragma once

#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>

#include <pb/Config.h>
#include <pb/infra/Traits.h>

namespace PB {
struct PlatformInfo {
  Path                          installationPath;
  Path                          localStatePath;
  std::pair<unsigned, unsigned> screenSize;

  static constexpr const char *COLLAGES_TEMPLATES_RESOURCES_NAME =
      "collages-templates-resources";

  Path projectFolderPath() const { return localStatePath / "projects"; }

  Path projectSupportFolder(boost::uuids::uuid projectId) const
  {
    return projectFolderPath() / boost::uuids::to_string(projectId);
  }

  Path collagesFolder(boost::uuids::uuid projectId) const
  {
    return projectSupportFolder(projectId) / COLLAGES_TEMPLATES_RESOURCES_NAME /
           "";
  }

  Path projectPath(std::string name) const
  {
    return localStatePath / "projects" / (name + OneConfig::BOOK_EXTENSION);
  }

  Path databasePath() const
  {
    return localStatePath / OneConfig::DATABASE_NAME;
  }

  Path logPath() const { return localStatePath / "log.txt"; }

  Path thumbnailByHash(boost::uuids::uuid projectId, std::string hash,
                       std::string extension = OneConfig::JPG_EXTENSION) const
  {
    return projectSupportFolder(projectId) / "thumbnail-images" /
           (hash + extension);
  }

  // TODO: rename this to something more meaningful
  std::tuple<Path, Path, Path, std::string>
  newThumbnailPaths(boost::uuids::uuid projectId, std::string hash = "") const
  {
    if (hash.empty()) {
      hash = boost::uuids::to_string(boost::uuids::random_generator()());
    }
    return std::make_tuple(projectSupportFolder(projectId) /
                               "thumbnail-images" / (hash + ".jpg"),
                           projectSupportFolder(projectId) /
                               "thumbnail-images" / (hash + "-m.jpg"),
                           projectSupportFolder(projectId) /
                               "thumbnail-images" / (hash + "-s.jpg"),
                           hash);
  }

  Path newTemporaryImage(boost::uuids::uuid projectId) const
  {
    return projectSupportFolder(projectId) /
           (boost::uuids::to_string(boost::uuids::random_generator()()) +
            ".jpg");
  }

  Path processedLutsPath() const
  {
    return localStatePath / OneConfig::PROCESSED_LUTS_FOLDER_NAME;
  }

  Path textFiles(std::string textFile) const
  {
    return installationPath / textFile;
  }

  Path waitImage() const { return installationPath / "others" / "wait.jpg"; }

  std::tuple<Path, Path, Path> waitThumbnails() const
  {
    return std::make_tuple(installationPath / "others" / "wait.jpg",
                           installationPath / "others" / "wait-m.jpg",
                           installationPath / "others" / "wait-s.jpg");
  }

  Path othersPath() const { return installationPath / "others"; }
};
} // namespace PB