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
  void configureProject(std::shared_ptr<Project> project)
  {
    mProject = project;
  }

  void configurePlatformInfo(std::shared_ptr<PlatformInfo> platformInfo)
  {
    mPlatformInfo = platformInfo;
  }

  void configureTaskCruncher(std::shared_ptr<TaskCruncher> taskCruncher)
  {
    mTaskCruncher = taskCruncher;
  }

  void configureExportListener(ExportListener *listener)
  {
    mListener = listener;
  }

  void exportPDFAlbum(std::string name, Path path)
  {
    auto                           pdfName = path / (name + ".pdf");
    std::shared_ptr<PdfExportTask> task = std::make_shared<PdfExportTask>(
        pdfName, mPlatformInfo->localStatePath, mProject->paperSettings,
        mProject->stagedImages()->stagedPhotos());
    task->configurePodofoListener(this);
    start(name, task);
  }

  void exportPDFLibharu(std::string name, Path path)
  {
    auto pdfName = path / (name + "-libharu" + ".pdf");
    std::shared_ptr<PdfLibharuExportTask> task =
        std::make_shared<PdfLibharuExportTask>(
            pdfName, mPlatformInfo->localStatePath, mProject->paperSettings,
            mProject->stagedImages()->stagedPhotos());
    task->configureLibharuListener(this);
    start(name, task);
  }

  void exportJPGAlbum(std::string name, Path path)
  {
    auto newFolder = path / name;
    if (std::filesystem::exists(newFolder)) {
      mListener->onExportAborted(name);
    }
    else {
      auto success = std::filesystem::create_directories(newFolder);
      PBDev::basicAssert(success);

      std::shared_ptr<JpgExport> task =
          std::make_shared<JpgExport>(newFolder, mProject->paperSettings,
                                      mProject->stagedImages()->stagedPhotos());
      task->configureJpgListener(this);
      start(name, task);
    }
  }

  void onExportComplete(PBDev::MapReducerTaskId id) override
  {
    mListener->onExportComplete(mPendingTaskNames.at(id));
    mPendingTasks.erase(id);
    mPendingTaskNames.erase(id);
  }

  void onExportAborted(PBDev::MapReducerTaskId id) override
  {
    mListener->onExportAborted(mPendingTaskNames.at(id));
    mPendingTasks.erase(id);
    mPendingTaskNames.erase(id);
  }

  void onExportUpdate(PBDev::MapReducerTaskId id) override
  {
    auto name = mPendingTaskNames.at(id);
    mListener->onExportUpdate(name);
  }

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

  void start(std::string name, std::shared_ptr<MapReducer> task)
  {
    PBDev::MapReducerTaskId id(RuntimeUUID::newUUID());
    task->assignUuid(id);

    mPendingTasks.emplace(id, task);
    mPendingTaskNames.emplace(id, name);

    auto stopSource =
        mTaskCruncher->crunch("export-logic", *mPendingTasks.at(id),
                              PBDev::ProgressJobName{"export"});
    UNUSED(stopSource);
  }
};
} // namespace PB