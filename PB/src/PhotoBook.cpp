#include <pb/PhotoBook.h>
#include <pb/util/FileInfo.h>

namespace PB {
PhotoBook::PhotoBook(GradualControllableListener &listener)
    : GradualControllable(listener)
{
}

void PhotoBook::addMedia(std::string const &path)
{
  PB::Path fsPath = path;
  auto     result = FileInfo::validInputRootPath(fsPath);
  std::visit(
      overloaded{
          [this](PB::Path const &path) { mMediaFolders.push_back(path); },
                        [mListener = &mListener](Error error) {
                          mListener->onError(error);
                        }},
             result);
}
void PhotoBook::setOutputPath(std::string const &path)
{
  PB::Path fsPath = path;
  auto     result = FileInfo::validOutputRootPath(fsPath);
  std::visit(
      overloaded{
          [this](PB::Path const &path) mutable { mOutputPath = path; },
          [mListener = &mListener](Error error) { mListener->onError(error); }},
      result);
}

auto PhotoBook::mapImages(std::string const &root)
    -> std::vector<std::filesystem::path>
{
  if (!fileMapper) {
    auto newFileMap = FilesMap(root);
    fileMapper = newFileMap;
  }
  else {
    fileMapper->setRootDirectory(root);
  }
  return fileMapper->map();
}

auto PhotoBook::loadImage(std::string const &path) -> std::optional<cv::Mat>
{
  return ImageReader::defaultRead()(path);
}

void PhotoBook::exportImage(std::string const &path) {}

void PhotoBook::doStart()
{
  
}

} // namespace PB