#pragma once

#include <hpdf.h>

#include <pb/MapReducer.h>
#include <pb/export/ExportLogic.h>
#include <pb/image/VirtualImage.h>
#include <pb/util/Traits.h>

namespace PB {

class PdfLibharuExportTask final : public MapReducer {
public:
  PdfLibharuExportTask(
      Path exportPdfPath, Path localStatePath, PaperSettings paperSettings,
      std::vector<std::shared_ptr<VirtualImage>> const &stagedImages);
  ~PdfLibharuExportTask() = default;

  void setListener(ExportLogicListener *listener) { mListener = listener; }

  bool stoppingCondition() const;
  int  stepsCount() const;
  void taskStep();

  std::string name() const;

  std::optional<IdentifyableFunction>
  getTask(std::stop_token stopToken) override;

  void onTaskFinished(PBDev::MapReducerTaskId id) override;

  unsigned taskCount() const override { return (unsigned)mStagedImages.size(); }

private:
  static constexpr const char *TEMPORARY_PHOTO = "temporary-photo.jpg";

  void writeImage(Path inputPath, Path outputPath) const;

  ExportLogicListener                       *mListener = nullptr;
  int                                        mIndex = 0;
  std::vector<std::shared_ptr<VirtualImage>> mStagedImages;
  Path                                       mLocalStatePath;
  PaperSettings                              mPaperSettings;
  Path                                       mPdfPath;
  HPDF_Doc                                   mPDFFile;
  bool                                       mCrunchedFlag = false;
  std::stop_token                            mStopToken;
};
} // namespace PB