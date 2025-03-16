#pragma once

#include <pb/Platform.h>
#include <pb/components/Project.h>
#include <pb/entities/GenericImage.h>
#include <pb/image/ImageFactory.h>
#include <pb/image/ImageOperations.h>
#include <pb/infra/Traits.h>

DECLARE_STRONG_UUID(EffectId)

namespace PB::Service {
class EffectsServiceListener {
public:
  virtual void onEffectsApplied(PBDev::EffectId effectId,
                                GenericImagePtr image) = 0;

  virtual void onEffectsAppliedInplace(PBDev::EffectId effectId) = 0;

  virtual void onEffectsApplicationError(PBDev::EffectId effectId,
                                         PB::ErrorCode) = 0;
};

class EffectsService final {
public:
  EffectsService() = default;
  EffectsService(const EffectsService &) = default;
  EffectsService(EffectsService &&) = default;
  EffectsService &operator=(const EffectsService &) = default;

  ~EffectsService() = default;

  void configureEffectsServiceListener(EffectsServiceListener *listener);
  void configurePlatformInfo(std::shared_ptr<PlatformInfo> platformInfo);
  void configureProject(IdentifiableProject project);
  void configureImageFactory(std::shared_ptr<ImageFactory> imageFactory);
  void configureTaskCruncher(std::shared_ptr<TaskCruncher> taskCruncher);

  void apply(PBDev::EffectId effectId, GenericImagePtr image, float saturation,
             float brightness, float contrast, bool inplace);

private:
  EffectsServiceListener       *mListener = nullptr;
  std::shared_ptr<PlatformInfo> mPlatformInfo = nullptr;
  IdentifiableProject           mProject = nullptr;
  std::shared_ptr<ImageFactory> mImageFactory = nullptr;
  std::shared_ptr<TaskCruncher> mTaskCruncher = nullptr;

  void applyInternal(PBDev::EffectId effectId, GenericImagePtr image,
                     float saturation, float brightness, float contrast,
                     bool inplace);
};
} // namespace PB::Service
