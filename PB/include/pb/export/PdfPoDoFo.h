#pragma once

#include <pb/components/MapReducer.h>
#include <pb/entities/GenericImage.h>
#include <pb/export/ExportLogic.h>
#include <pb/project/Project.h>

#include <podofo/podofo.h>

namespace PB {

class PdfExportTask final : public MapReducer {
public:
  PdfExportTask(Path exportPdfPath, Path localStatePath,
                PaperSettings                       paperSettings,
                std::vector<GenericImagePtr> const &stagedImages);
  ~PdfExportTask() = default;

  void setListener(ExportLogicListener *listener) { mListener = listener; }

  bool stoppingCondition() const;
  int  stepsCount() const;
  void taskStep();

  std::string name() const;

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
};

} // namespace PB
