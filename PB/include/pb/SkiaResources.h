#pragma once

#include <unordered_map>

#include <modules/skresources/include/SkResources.h>

#include <pb/util/Traits.h>

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
                     sk_sp<skresources::FileResourceProvider>>
      mResourceProviders;
};
} // namespace PB
