#pragma once

#pragma warning(push)
#pragma warning(disable : 5054)
#pragma warning(disable : 4127)
#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#pragma warning(pop)

#include <filesystem>
#include <tuple>
#include <vector>

#include <pb/Config.h>
#include <pb/config/Log.h>
#include <pb/util/Util.h>

namespace PB {
enum class ExportType { PDF, JPG_COLLECTION, HTML };

class ImageSetWriter final {
public:
  ImageSetWriter() : mExportType(ExportType::JPG_COLLECTION){};

  explicit ImageSetWriter(ExportType type) : mExportType(type) {}

  ImageSetWriter(ImageSetWriter const &) = delete;
  ImageSetWriter(ImageSetWriter &&) = delete;
  ImageSetWriter &operator=(ImageSetWriter const &other) = delete;

  ~ImageSetWriter() = default;

  template <typename T>
  void exportJPGCollection(T const &images, Path path,
                           std::string const &projectName)
  {
    path = path / Path(projectName);
    if (!std::filesystem::create_directories(path)) {
      // on error
    }
    writeImages(images);
  }

  template <template <typename> typename T>
  void writeImages(T<std::shared_ptr<cv::Mat>> const &images, Path folder) const
  {
    int counter = 0;

    for (auto imagePtr : images) {
      auto path = makePath(folder, counter);
      write(path, imagePtr);
      counter++;
    }
  }

  template <template <typename, typename> typename T>
  void writeImages(T<Path, std::shared_ptr<cv::Mat>> const &images,
                   Path                                     folder) const
  {
    int counter = 0;
    for (auto [key, value] : images) {
      Unused(key);
      auto path = makePath(folder, counter);
      write(path, value);
      counter++;
    }
  }

  void write(Path const &path, std::shared_ptr<cv::Mat> image)
  {
    bool success = cv::imwrite(path.string(), *image);
    if (!success) {
      printDebug("File could not be saved to %s\n", path.string().c_str());
    }
  }

private:
  Path makePath(Path path, int counter) const
  {
    const std::string prefix = "image_";
    std::string       fileNameStr =
        prefix + std::to_string(counter) + Context::JPG_EXTENSION;
    std::filesystem::path file(fileNameStr);
    path = path / file;
    return path;
  }

  ExportType mExportType;
};
} // namespace PB