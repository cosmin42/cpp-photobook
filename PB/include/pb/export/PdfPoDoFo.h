#pragma once

#include <pb/MapReducer.h>
#include <pb/export/ExportLogic.h>
#include <pb/image/VirtualImage.h>
#include <pb/project/Project.h>

#include <podofo/podofo.h>

namespace PB {

class PdfExportTask final : public MapReducer {
public:
  PdfExportTask(Path exportPdfPath, Path localStatePath,
                PaperSettings                                     paperSettings,
                std::vector<std::shared_ptr<VirtualImage>> const &stagedImages);
  ~PdfExportTask() = default;

  void setListener(ExportLogicListener *listener) { mListener = listener; }

  bool stoppingCondition() const;
  int  stepsCount() const;
  void taskStep();

  std::string name() const;

  virtual std::optional<IdentifyableFunction>
  getNext(std::stop_token stopToken) override;

  virtual void onFinished(const boost::uuids::uuid id) override;

private:
  static constexpr const char *TEMPORARY_PHOTO = "temporary-photo.jpg";

  void writeImage(Path inputPath, Path outputPath) const;

  ExportLogicListener                         *mListener = nullptr;
  int                                          mIndex = 0;
  std::vector<std::shared_ptr<VirtualImage>>   mStagedImages;
  std::shared_ptr<PoDoFo::PdfStreamedDocument> mDocument = nullptr;
  Path                                         mLocalStatePath;
  PaperSettings                                mPaperSettings;
  Path                                         mPdfPath;
  bool                                         mCrunchedFlag = false;
  std::stop_token                              mStopToken;
};

} // namespace PB
