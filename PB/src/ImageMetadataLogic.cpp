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

    Exiv2::ExifKey            key("Exif.Image.Orientation");
    Exiv2::ExifData::iterator pos = mExifData.findKey(key);

    if (pos != mExifData.end() && pos->count() > 0) {
      // Retrieve the orientation value
      mOrientation = pos->toUint32();
    }
  }
  catch (Exiv2::Error &e) {
    UNUSED(e);
    PBDev::basicAssert(false);
  }
}

auto ImageMetadataLogic::isRotated90Degrees() const -> bool
{
  return mOrientation == 6 || mOrientation == 8;
}

unsigned ImageMetadataLogic::width()
{
  if (isRotated90Degrees()) {
    return mImage->pixelHeight();
  }
  return mImage->pixelWidth();
}

unsigned ImageMetadataLogic::height()
{
  if (isRotated90Degrees()) {
    return mImage->pixelWidth();
  }
  return mImage->pixelHeight();
}

} // namespace PB
