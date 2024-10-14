#pragma once

#include <string>

#include <pb/components/MapReducer.h>
#include <pb/services/ProgressService.h>
#include <pb/tasks/ParallelTaskConsumer.h>

using namespace PB::Service;

namespace PB {
class TaskCruncher final {
public:
  TaskCruncher() = default;
  ~TaskCruncher() = default;

  TaskCruncher(const TaskCruncher &) = delete;
  TaskCruncher &operator=(const TaskCruncher &) = delete;
  TaskCruncher(TaskCruncher &&) = delete;

  void
  configureProgressService(std::shared_ptr<ProgressService> progressService)
  {
    mProgressService = progressService;
  }

  void registerPTC(const std::string poolName, unsigned thrreadsCount);

  void crunch(const std::string poolName, MapReducer &mapper,
              PBDev::ProgressJobName progressName);

  void crunch(std::function<void()> f);

  void abort();

private:
  std::stop_source mStopSource;
  std::unordered_map<std::string, std::unique_ptr<PBDev::ParallelTaskConsumer>>
                                   mPTC;
  std::shared_ptr<ProgressService> mProgressService = nullptr;
};
} // namespace PB
