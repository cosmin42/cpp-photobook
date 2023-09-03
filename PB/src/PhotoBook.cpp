#include <pb/PhotoBook.h>
#include <pb/common/Log.h>
#include <pb/util/FileInfo.h>

namespace PB {

void PhotoBook::addMedia(std::string const &path)
{
  PB::Path fsPath = path;
  auto     result = FileInfo::validInputRootPath(fsPath);
  std::visit(
      overloaded{[this](PB::Path const &path) {
                   printDebug("Add media %s\n", path.c_str());
                   mMediaFolders.insert({path, MediaMapper(path, []() {})});
                 },
                 [this](Error error) { mListener->onError(error); }},
      result);
}
void PhotoBook::setOutputPath(std::string const &path)
{
  PB::Path fsPath = path;
  auto     result = FileInfo::validOutputRootPath(fsPath);
  std::visit(
      overloaded{[this](PB::Path const &path) mutable { mOutputPath = path; },
                 [this](Error error) { mListener->onError(error); }},
      result);
}

auto PhotoBook::loadImage(std::string const &path) -> std::optional<cv::Mat>
{
  return ImageReader::defaultRead()(path);
}

void PhotoBook::exportImage(std::string const &path) {}

void PhotoBook::setPhotoBookListener(
    std::shared_ptr<GradualControllableListener> listener)
{
  mListener = listener;
}

} // namespace PB