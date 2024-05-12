#pragma once

#include <memory>
#include <vector>

#include <pb/RuntimeUUID.h>
#include <pb/TaskCruncher.h>
#include <pb/image/VirtualImage.h>
#include <pb/project/Project.h>

namespace PB {

// TODO: Move this to a file that is inherited by the tasks.
class ExportLogicListener {
public:
  virtual void onExportComplete(boost::uuids::uuid id) = 0;
  virtual void onExportAborted(boost::uuids::uuid id) = 0;
  virtual void onExportUpdate(boost::uuids::uuid id) = 0;
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
    auto newId = RuntimeUUID::newUUID();
    mPendingTasks.emplace(newId, task);
    mPendingTaskNames.emplace(newId, name);

    mTaskCruncher->crunch("export-logic", *mPendingTasks.at(newId));
  }

  void onExportComplete(boost::uuids::uuid id) override
  {
    mListener->onExportComplete(mPendingTaskNames.at(id));
    mPendingTasks.erase(id);
    mPendingTaskNames.erase(id);
  }

  void onExportAborted(boost::uuids::uuid id) override
  {
    mListener->onExportAborted(mPendingTaskNames.at(id));
    mPendingTasks.erase(id);
    mPendingTaskNames.erase(id);
  }

  void onExportUpdate(boost::uuids::uuid id) override
  {
    mListener->onExportUpdate(mPendingTaskNames.at(id));
  }

private:
  ExportListener                   *mListener;
  std::shared_ptr<PB::Project>      mProject;
  std::shared_ptr<PB::PlatformInfo> mPlatformInfo;

  std::vector<std::shared_ptr<VirtualImage>> mPtrImages;
  std::shared_ptr<TaskCruncher>              mTaskCruncher;
  std::unordered_map<boost::uuids::uuid, std::shared_ptr<MapReducer>,
                     boost::hash<boost::uuids::uuid>>
      mPendingTasks;
  std::unordered_map<boost::uuids::uuid, std::string,
                     boost::hash<boost::uuids::uuid>>
      mPendingTaskNames;
};
} // namespace PB