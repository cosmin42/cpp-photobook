#include <pb/ImageMetadataLogic.h>

namespace PB {
ImageMetadataLogic::ImageMetadataLogic(Path path) : mPath(path) {}

void ImageMetadataLogic::inspect()
{
  auto filename = mPath.string();
  try {
    // Load the image
    Exiv2::Image::UniquePtr image = Exiv2::ImageFactory::open(filename);
    assert(image.get() != 0);

    // Read metadata
    image->readMetadata();

    // Get image size
    mExifData = image->exifData();
    if (mExifData.empty()) {
      std::string error(filename);
      error += ": No Exif data found in the file";
      throw Exiv2::Error(Exiv2::ErrorCode::kerErrorMessage, error);
    }
  }
  catch (Exiv2::Error &e) {
    UNUSED(e);
    PBDev::basicAssert(false);
  }
}

unsigned ImageMetadataLogic::width()
{
  Exiv2::Exifdatum &width = mExifData["Exif.Photo.PixelXDimension"];
  return width.toUint32();
}

unsigned ImageMetadataLogic::height()
{
  Exiv2::Exifdatum &height = mExifData["Exif.Photo.PixelYDimension"];
  return height.toUint32();
}

} // namespace PB
