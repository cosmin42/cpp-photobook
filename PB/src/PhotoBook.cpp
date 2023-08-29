#include <PhotoBook.h>

namespace PB {
PhotoBook::PhotoBook(PhotoBookListener &listener) : mListener(listener) {}

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

void PhotoBook::exportImage(std::string const &path) {}
} // namespace PB