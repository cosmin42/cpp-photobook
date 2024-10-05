#include <pb/entities/GenericImage.h>

namespace PB {
GenericImage::GenericImage(Path projectPath, std::string hash)
    : mProjectPath(std::move(projectPath)), mHash(std::move(hash))
{
}

Path GenericImage::full() const
{
  return mProjectPath / (mHash + "-full" + ".jpg");
}

Path GenericImage::medium() const
{
  return mProjectPath / (mHash + "-medium" + ".jpg");
}

Path GenericImage::smaLL() const
{
  return mProjectPath / (mHash + "-small" + ".jpg");
}

std::string GenericImage::defaultHash() { return ""; }

} // namespace PB