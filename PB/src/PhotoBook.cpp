#include <PhotoBook.h>
#include <util/FileInfo.h>

namespace PB {
PhotoBook::PhotoBook(PhotoBookListener &listener) : mListener(listener) {}

void PhotoBook::setInputPath(std::string const &path)
{
  PB::Path fsPath = path;
  auto     result = FileInfo::validInputRootPath(fsPath);
  std::visit(overloaded{[inputPath = &mInputPath](
                            PB::Path const &path) mutable { inputPath = path; },
                        [mListener = &mListener](Error error) {
                          mListener->onError(error);
                        }},
             result);
}
void PhotoBook::setOutputPath(std::string const &path)
{
  PB::Path fsPath = path;
  auto     result = FileInfo::validOutputRootPath(fsPath);
  std::visit(overloaded{[inputPath = &mInputPath](
                            PB::Path const &path) mutable { inputPath = path; },
                        [mListener = &mListener](Error error) {
                          mListener->onError(error);
                        }},
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
} // namespace PB