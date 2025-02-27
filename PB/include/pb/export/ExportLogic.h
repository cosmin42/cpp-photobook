#pragma once

#include <memory>
#include <vector>

#include <pb/components/RuntimeUUID.h>
#include <pb/components/TaskCruncher.h>
#include <pb/entities/GenericImage.h>
#include <pb/components/Project.h>

DECLARE_STRONG_UUID(ExportLogicReducersId)

namespace PB {

// TODO: Move this to a file that is inherited by the tasks.
class ExportLogicListener {
public:
  virtual void onExportComplete(PBDev::MapReducerTaskId) = 0;
  virtual void onExportAborted(PBDev::MapReducerTaskId) = 0;
  virtual void onExportUpdate(PBDev::MapReducerTaskId) = 0;
};

class ExportListener {
public:
  virtual void onExportComplete(std::string name) = 0;
  virtual void onExportAborted(std::string name) = 0;
  virtual void onExportUpdate(std::string name) = 0;
};

class ExportLogic final : public ExportLogicListener {
public:
  void configure(std::shared_ptr<PB::Project>      project,
                 std::shared_ptr<PB::PlatformInfo> platformInfo)
  {
    mProject = project;
    mPlatformInfo = platformInfo;
  }

  void setTaskCruncher(std::shared_ptr<TaskCruncher> taskCruncher)
  {
    mTaskCruncher = taskCruncher;
  }

  void setExportListener(ExportListener *listener) { mListener = listener; }

  void start(std::string name, std::shared_ptr<MapReducer> task)
  {
    PBDev::MapReducerTaskId id(RuntimeUUID::newUUID());
    task->assignUuid(id);

    mPendingTasks.emplace(id, task);
    mPendingTaskNames.emplace(id, name);

    auto stopSource = mTaskCruncher->crunch("export-logic", *mPendingTasks.at(id),
                          PBDev::ProgressJobName{"export"});
    UNUSED(stopSource);
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
  ExportListener                   *mListener;
  std::shared_ptr<PB::Project>      mProject;
  std::shared_ptr<PB::PlatformInfo> mPlatformInfo;

  std::vector<GenericImagePtr>  mPtrImages;
  std::shared_ptr<TaskCruncher> mTaskCruncher;
  std::unordered_map<PBDev::MapReducerTaskId, std::shared_ptr<MapReducer>,
                     boost::hash<PBDev::MapReducerTaskId>>
      mPendingTasks;
  std::unordered_map<PBDev::MapReducerTaskId, std::string,
                     boost::hash<PBDev::MapReducerTaskId>>
      mPendingTaskNames;
};
} // namespace PB