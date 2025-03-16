#include <pb/export/ExportService.h>

namespace PB {
void ExportService::configureProject(IdentifiableProject project)
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
  auto                           pdfPath = path / (name + ".pdf");
  std::shared_ptr<PdfExportTask> task = std::make_shared<PdfExportTask>(
      pdfPath, mPlatformInfo->localStatePath, mProject->value.paperSettings,
      mProject->value.stagedImages()->stagedPhotos());
  task->configurePodofoListener(this);
  task->configurePlatformInfo(mPlatformInfo);
  task->configureProject(mProject);
  start(name, task);
}

void ExportService::exportPDFLibharu(std::string name, Path path)
{
  auto pdfName = path / (name + "-libharu" + ".pdf");
  std::shared_ptr<PdfLibharuExportTask> task =
      std::make_shared<PdfLibharuExportTask>(
          pdfName, mPlatformInfo->localStatePath, mProject->value.paperSettings,
          mProject->value.stagedImages()->stagedPhotos());
  task->configureLibharuListener(this);
  task->configurePlatformInfo(mPlatformInfo);
  task->configureProject(mProject);
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

    std::shared_ptr<JpgExport> task = std::make_shared<JpgExport>(
        newFolder, mProject->value.paperSettings,
        mProject->value.stagedImages()->stagedPhotos());
    task->configureJpgListener(this);
    task->configurePlatformInfo(mPlatformInfo);
    task->configureProject(mProject);
    start(name, task);
  }
}

void ExportService::onExportComplete(Path path)
{
  mListener->onExportComplete(path);
  mPendingTasks.erase(path);
  mPendingPaths.erase(path);
}

void ExportService::onExportAborted(Path path)
{
  mListener->onExportAborted(path);
  mPendingTasks.erase(path);
  mPendingPaths.erase(path);
}

void ExportService::onExportUpdate(Path path)
{
  mListener->onExportUpdate(path);
}

void ExportService::start(Path path, std::shared_ptr<MapReducer> task)
{
  mPendingTasks[path] = task;
  mPendingPaths.insert(path);

  auto stopSource =
      mTaskCruncher->crunch("export-logic", *mPendingTasks.at(path),
                            PBDev::ProgressJobName{"export"});
  UNUSED(stopSource);
}
} // namespace PB