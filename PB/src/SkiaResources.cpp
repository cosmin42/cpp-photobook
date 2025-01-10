#include <pb/components/SkiaResources.h>

#include <src/utils/SkOSPath.h>

namespace PB {

// TODO: Improve design for this class
PBDev::SkiaResourcesId SkiaResources::addResource(Path path)
{
  for (auto const &[id, path] : mRegisteredPaths) {
    if (path == path) {
      return id;
    }
  }

  auto assetPath = SkOSPath::Dirname(path.string().c_str());

  auto provider = skresources::FileResourceProvider::Make(assetPath);
  PBDev::SkiaResourcesId id(RuntimeUUID::newUUID());
  mResourceProviders[id] = provider;
  mRegisteredPaths[id] = path;
  return id;
}

void SkiaResources::removeResource(PBDev::SkiaResourcesId id)
{
  mResourceProviders.erase(id);
  mRegisteredPaths.erase(id);
}

sk_sp<skresources::FileResourceProvider>
SkiaResources::resourceProvider(PBDev::SkiaResourcesId id)
{
  return mResourceProviders.at(id);
}

} // namespace PB
