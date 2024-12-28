#include <pb/entities/GenericImage.h>

#include <boost/uuid/uuid_io.hpp>

namespace PB {
Path GenericImage::sProjectPath = Path();

GenericImage::GenericImage(std::string hash) : mHash(std::move(hash)) {}

Path GenericImage::full() const { return sProjectPath / (mHash + "" + ".jpg"); }

Path GenericImage::medium() const
{
  return sProjectPath / (imageName() + "-m" + ".jpg");
}

Path GenericImage::smaLL() const
{
  return sProjectPath / (imageName() + "-s" + ".jpg");
}

std::string GenericImage::defaultHash()
{
  return "placeholder" + boost::uuids::to_string(RuntimeUUID::newUUID());
}

std::string GenericImage::imageName() const
{
  if (mHash.starts_with("placeholder")) {
    return "wait";
  }
  else {
    return mHash;
  }
}

} // namespace PB