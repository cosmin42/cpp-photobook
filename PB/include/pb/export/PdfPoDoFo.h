#pragma once

#include <pb/components/MapReducer.h>
#include <pb/components/Project.h>
#include <pb/entities/GenericImage.h>
#include <pb/export/ExportTaskListener.h>

#include <podofo/podofo.h>

namespace PB {

class PdfExportTask final : public MapReducer {
public:
  PdfExportTask(Path exportPdfPath, Path localStatePath,
                PaperSettings                       paperSettings,
                std::vector<GenericImagePtr> const &stagedImages);
  ~PdfExportTask() = default;

  void configurePodofoListener(ExportLogicListener *listener)
  {
    mListener = listener;
  }

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

  virtual std::optional<IdentifyableFunction>
  getTask(std::stop_token stopToken) override;

  virtual void onTaskFinished(PBDev::MapReducerTaskId id) override;

  unsigned taskCount() const override { return (unsigned)mStagedImages.size(); }

private:
  static constexpr const char *TEMPORARY_PHOTO = "temporary-photo.jpg";

  void writeImage(Path inputPath, Path outputPath) const;

  ExportLogicListener                         *mListener = nullptr;
  int                                          mIndex = 0;
  std::vector<GenericImagePtr>                 mStagedImages;
  std::shared_ptr<PoDoFo::PdfStreamedDocument> mDocument = nullptr;
  Path                                         mLocalStatePath;
  PaperSettings                                mPaperSettings;
  Path                                         mPdfPath;
  bool                                         mCrunchedFlag = false;
  std::stop_token                              mStopToken;
  std::shared_ptr<IdentifyableProject>         mProject = nullptr;
  std::shared_ptr<PlatformInfo>                mPlatformInfo = nullptr;
};

} // namespace PB
