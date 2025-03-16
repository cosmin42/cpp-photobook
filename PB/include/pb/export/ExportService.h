#pragma once

#include <memory>
#include <vector>

#include <pb/components/Project.h>
#include <pb/components/RuntimeUUID.h>
#include <pb/entities/GenericImage.h>
#include <pb/export/Jpg.h>
#include <pb/export/PdfLibharu.h>
#include <pb/export/PdfPoDoFo.h>
#include <pb/infra/TaskCruncher.h>

DECLARE_STRONG_UUID(ExportLogicReducersId)

namespace PB {

class ExportListener {
public:
  virtual void onExportComplete(Path) = 0;
  virtual void onExportAborted(Path) = 0;
  virtual void onExportUpdate(Path) = 0;
};

class ExportService final : public ExportLogicListener {
public:
  void configureProject(IdentifiableProject project);

  void configurePlatformInfo(std::shared_ptr<PlatformInfo> platformInfo);

  void configureTaskCruncher(std::shared_ptr<TaskCruncher> taskCruncher);

  void configureExportListener(ExportListener *listener);

  void exportPDFAlbum(std::string name, Path path);

  void exportPDFLibharu(std::string name, Path path);

  void exportJPGAlbum(std::string name, Path path);

  void onExportComplete(Path) override;

  void onExportAborted(Path) override;

  void onExportUpdate(Path) override;

private:
  ExportListener               *mListener = nullptr;
  IdentifiableProject           mProject = nullptr;
  std::shared_ptr<PlatformInfo> mPlatformInfo = nullptr;
  std::shared_ptr<TaskCruncher> mTaskCruncher = nullptr;

  std::unordered_map<Path, std::shared_ptr<MapReducer>> mPendingTasks;
  std::set<Path>                                        mPendingPaths;

  void start(Path path, std::shared_ptr<MapReducer> task);
};
} // namespace PB