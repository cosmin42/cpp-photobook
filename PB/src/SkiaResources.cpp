#include <pb/components/SkiaResources.h>

#include <src/utils/SkOSPath.h>

namespace PB {

PBDev::SkiaResourcesId SkiaResources::addResource(Path path)
{
  auto assetPath = SkOSPath::Dirname(path.string().c_str());

  auto provider = skresources::FileResourceProvider::Make(assetPath);
  PBDev::SkiaResourcesId id(RuntimeUUID::newUUID());
  mResourceProviders[id] = provider;
  return id;
}

void SkiaResources::removeResource(PBDev::SkiaResourcesId id)
{
  mResourceProviders.erase(id);
}

sk_sp<skresources::FileResourceProvider>
SkiaResources::resourceProvider(PBDev::SkiaResourcesId id)
{
  return mResourceProviders.at(id);
}

} // namespace PB
