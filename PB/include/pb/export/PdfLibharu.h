#pragma once

#include <hpdf.h>

#include <pb/components/MapReducer.h>
#include <pb/components/Project.h>
#include <pb/entities/GenericImage.h>
#include <pb/entities/PaperSettings.h>
#include <pb/export/ExportTaskListener.h>
#include <pb/infra/Traits.h>

namespace PB {

class PdfLibharuExportTask final : public MapReducer {
public:
  PdfLibharuExportTask(Path exportPdfPath, Path localStatePath,
                       PaperSettings                       paperSettings,
                       std::vector<GenericImagePtr> const &stagedImages);
  ~PdfLibharuExportTask() = default;

  void configureLibharuListener(ExportLogicListener *listener)
  {
    mListener = listener;
  }

  void configureProject(IdentifiableProject project) { mProject = project; }

  void configurePlatformInfo(std::shared_ptr<PlatformInfo> platformInfo)
  {
    mPlatformInfo = platformInfo;
  }

  bool stoppingCondition() const;
  int  stepsCount() const;
  void taskStep();

  std::optional<IdentifyableFunction>
  getTask(std::stop_token stopToken) override;

  void onTaskFinished(PBDev::MapReducerTaskId id) override;

  unsigned taskCount() const override { return (unsigned)mStagedImages.size(); }

private:
  static constexpr const char *TEMPORARY_PHOTO = "temporary-photo.jpg";

  void writeImage(Path inputPath, Path outputPath) const;

  ExportLogicListener          *mListener = nullptr;
  int                           mIndex = 0;
  std::vector<GenericImagePtr>  mStagedImages;
  Path                          mPdfPath;
  Path                          mLocalStatePath;
  PaperSettings                 mPaperSettings;
  HPDF_Doc                      mPDFFile;
  bool                          mCrunchedFlag = false;
  std::stop_token               mStopToken;
  IdentifiableProject           mProject = nullptr;
  std::shared_ptr<PlatformInfo> mPlatformInfo = nullptr;
};
} // namespace PB