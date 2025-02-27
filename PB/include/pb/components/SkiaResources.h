#pragma once

#include <unordered_map>

#pragma warning(push)
#pragma warning(disable : 4244)
#pragma warning(disable : 4267)
#include <modules/skresources/include/SkResources.h>
#pragma warning(pop)

#include <pb/infra/Traits.h>

DECLARE_STRONG_UUID(SkiaResourcesId)

namespace PB {
class SkiaResources final {
public:
  SkiaResources() = default;
  ~SkiaResources() = default;

  PBDev::SkiaResourcesId addResource(Path path);
  void                   removeResource(PBDev::SkiaResourcesId id);

  sk_sp<skresources::FileResourceProvider>
  resourceProvider(PBDev::SkiaResourcesId id);

private:
  SkiaResources(const SkiaResources &) = delete;
  SkiaResources &operator=(const SkiaResources &) = delete;

  std::unordered_map<PBDev::SkiaResourcesId,
                     sk_sp<skresources::FileResourceProvider>,
                     boost::hash<PBDev::SkiaResourcesId>>
      mResourceProviders;
  std::unordered_map<PBDev::SkiaResourcesId, Path,
                     boost::hash<PBDev::SkiaResourcesId>>
      mRegisteredPaths;
};
} // namespace PB
