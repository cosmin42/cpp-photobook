#include <pb/entities/GenericImage.h>

namespace PB {
Path GenericImage::sProjectPath = Path();

GenericImage::GenericImage(std::string hash) : mHash(std::move(hash)) {}

Path GenericImage::full() const
{
  return sProjectPath / (mHash + "" + ".jpg");
}

Path GenericImage::medium() const
{
  return sProjectPath / (mHash + "-m" + ".jpg");
}

Path GenericImage::smaLL() const
{
  return sProjectPath / (mHash + "-s" + ".jpg");
}

std::string GenericImage::defaultHash() { return ""; }

} // namespace PB