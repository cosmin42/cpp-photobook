#include <PhotoBook.h>

namespace PB {
PhotoBook::PhotoBook(PhotoBookListener &listener) : mListener(listener) {}

void PhotoBook::loadImages(std::string const &root) {

}

void PhotoBook::exportIntermediaryImages(std::string const &path)
{

}
} // namespace PB