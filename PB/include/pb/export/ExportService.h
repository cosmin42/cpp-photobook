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
  virtual void onExportComplete(std::string name) = 0;
  virtual void onExportAborted(std::string name) = 0;
  virtual void onExportUpdate(std::string name) = 0;
};

class ExportService final : public ExportLogicListener {
public:
  void configureProject(std::shared_ptr<Project> project);

  void configurePlatformInfo(std::shared_ptr<PlatformInfo> platformInfo);

  void configureTaskCruncher(std::shared_ptr<TaskCruncher> taskCruncher);

  void configureExportListener(ExportListener *listener);

  void exportPDFAlbum(std::string name, Path path);

  void exportPDFLibharu(std::string name, Path path);

  void exportJPGAlbum(std::string name, Path path);

  void onExportComplete(PBDev::MapReducerTaskId id) override;

  void onExportAborted(PBDev::MapReducerTaskId id) override;

  void onExportUpdate(PBDev::MapReducerTaskId id) override;

private:
  ExportListener               *mListener;
  std::shared_ptr<Project>      mProject;
  std::shared_ptr<PlatformInfo> mPlatformInfo;

  std::vector<GenericImagePtr>  mPtrImages;
  std::shared_ptr<TaskCruncher> mTaskCruncher;
  std::unordered_map<PBDev::MapReducerTaskId, std::shared_ptr<MapReducer>,
                     boost::hash<PBDev::MapReducerTaskId>>
      mPendingTasks;
  std::unordered_map<PBDev::MapReducerTaskId, std::string,
                     boost::hash<PBDev::MapReducerTaskId>>
      mPendingTaskNames;

  void start(std::string name, std::shared_ptr<MapReducer> task);
};
} // namespace PB