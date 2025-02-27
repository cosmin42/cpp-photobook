#include <pb/export/ExportService.h>

namespace PB {
void ExportService::configureProject(std::shared_ptr<Project> project)
{
  mProject = project;
}

void ExportService::configurePlatformInfo(
    std::shared_ptr<PlatformInfo> platformInfo)
{
  mPlatformInfo = platformInfo;
}

void ExportService::configureTaskCruncher(
    std::shared_ptr<TaskCruncher> taskCruncher)
{
  mTaskCruncher = taskCruncher;
}

void ExportService::configureExportListener(ExportListener *listener)
{
  mListener = listener;
}

void ExportService::exportPDFAlbum(std::string name, Path path)
{
  auto                           pdfName = path / (name + ".pdf");
  std::shared_ptr<PdfExportTask> task = std::make_shared<PdfExportTask>(
      pdfName, mPlatformInfo->localStatePath, mProject->paperSettings,
      mProject->stagedImages()->stagedPhotos());
  task->configurePodofoListener(this);
  start(name, task);
}

void ExportService::exportPDFLibharu(std::string name, Path path)
{
  auto pdfName = path / (name + "-libharu" + ".pdf");
  std::shared_ptr<PdfLibharuExportTask> task =
      std::make_shared<PdfLibharuExportTask>(
          pdfName, mPlatformInfo->localStatePath, mProject->paperSettings,
          mProject->stagedImages()->stagedPhotos());
  task->configureLibharuListener(this);
  start(name, task);
}

void ExportService::exportJPGAlbum(std::string name, Path path)
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

void ExportService::onExportComplete(PBDev::MapReducerTaskId id)
{
  mListener->onExportComplete(mPendingTaskNames.at(id));
  mPendingTasks.erase(id);
  mPendingTaskNames.erase(id);
}

void ExportService::onExportAborted(PBDev::MapReducerTaskId id)
{
  mListener->onExportAborted(mPendingTaskNames.at(id));
  mPendingTasks.erase(id);
  mPendingTaskNames.erase(id);
}

void ExportService::onExportUpdate(PBDev::MapReducerTaskId id)
{
  auto name = mPendingTaskNames.at(id);
  mListener->onExportUpdate(name);
}

void ExportService::start(std::string name, std::shared_ptr<MapReducer> task)
{
  PBDev::MapReducerTaskId id(RuntimeUUID::newUUID());
  task->assignUuid(id);

  mPendingTasks.emplace(id, task);
  mPendingTaskNames.emplace(id, name);

  auto stopSource = mTaskCruncher->crunch("export-logic", *mPendingTasks.at(id),
                                          PBDev::ProgressJobName{"export"});
  UNUSED(stopSource);
}
} // namespace PB