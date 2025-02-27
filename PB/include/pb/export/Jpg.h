#pragma once

#include <pb/components/MapReducer.h>
#include <pb/entities/GenericImage.h>
#include <pb/export/ExportLogic.h>
#include <pb/components/Project.h>
#include <pb/util/Traits.h>

namespace PB {

// TODO: Change name to be consistent with the pdf tasks.
class JpgExport final : public MapReducer {
public:
  explicit JpgExport(Path root, PaperSettings paperSettings,
                     std::vector<GenericImagePtr> const &stagedImages);
  ~JpgExport() = default;

  void setListener(ExportLogicListener *listener) { mListener = listener; }
  void configureProject(std::shared_ptr<IdentifyableProject> project)
  {
    mProject = project;
  }

  void configurePlatformInfo(std::shared_ptr<PlatformInfo> platformInfo)
  {
    mPlatformInfo = platformInfo;
  }

  bool stoppingCondition() const;
  int  stepsCount() const;
  void taskStep();

  std::string name() const;

  std::optional<IdentifyableFunction>
  getTask(std::stop_token stopToken) override;

  void onTaskFinished(PBDev::MapReducerTaskId id) override;

  unsigned taskCount() const override { return (unsigned)mStagedImages.size(); }

private:
  struct JPG_TEMPLATE_PARAMS {
    static constexpr const char *JPG_NAME_TEMPLATE = "PHOTO_0000000.JPG";
    static constexpr unsigned    PLACEHOLDER_SIZE = 7;
    static constexpr unsigned    PLACEHOLDER_OFFSET = 6;
  };

  std::string makeName(unsigned counter);
  void        writeImage(Path inputPath, Path outputPath) const;

  ExportLogicListener                 *mListener = nullptr;
  unsigned                             mIndex = 0;
  Path                                 mRoot;
  PaperSettings                        mPaperSettings;
  std::vector<GenericImagePtr>         mStagedImages;
  bool                                 mCrunchedFlag = false;
  std::stop_token                      mStopToken;
  std::shared_ptr<IdentifyableProject> mProject = nullptr;
  std::shared_ptr<PlatformInfo>        mPlatformInfo = nullptr;
};
} // namespace PB