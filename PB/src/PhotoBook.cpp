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
                   printDebug("Add media %s\n", path.string().c_str());
                   mMediaFolders.insert({path, MediaMapper(path, mListener)});
                 },
                 [this](Error error) { mListener.doError(error); }},
      result);

  mMediaFolders.at(fsPath).start();
}
void PhotoBook::setOutputPath(std::string const &path)
{
  PB::Path fsPath = path;
  auto     result = FileInfo::validOutputRootPath(fsPath);
  std::visit(
      overloaded{[this](PB::Path const &path) mutable { mOutputPath = path; },
                 [this](Error error) { mListener.doError(error); }},
      result);
}

auto PhotoBook::loadImage(std::string const &path) -> std::optional<cv::Mat>
{
  return ImageReader::defaultRead()(path);
}

auto PhotoBook::rootPaths() const -> std::vector<Path> {
  std::vector<Path> result;
  for (auto& [key, value] : mMediaFolders)
  {
    result.push_back(key);
  }
  return result;
}

void PhotoBook::exportImage(std::string const &path) {}

} // namespace PB