#include <pb/ImageMetadataLogic.h>

namespace PB {
ImageMetadataLogic::ImageMetadataLogic(Path path) : mPath(path) {}

void ImageMetadataLogic::inspect()
{
  auto filename = mPath.string();
  try {
    // Load the image
    mImage = Exiv2::ImageFactory::open(filename);
    assert(mImage.get() != 0);

    // Read metadata
    mImage->readMetadata();

    // Get image size
    mExifData = mImage->exifData();
  }
  catch (Exiv2::Error &e) {
    UNUSED(e);
    PBDev::basicAssert(false);
  }
}

unsigned ImageMetadataLogic::width() { return mImage->pixelWidth(); }

unsigned ImageMetadataLogic::height() { return mImage->pixelHeight(); }

} // namespace PB
