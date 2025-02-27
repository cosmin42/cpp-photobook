#pragma once

#include <string>

#include <pb/components/MapReducer.h>
#include <pb/services/ProgressService.h>
#include <pb/infra/ParallelTaskConsumer.h>

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

  [[nodiscard]] std::stop_source crunch(const std::string      poolName,
                                        MapReducer            &mapper,
                                        PBDev::ProgressJobName progressName);

  void crunch(std::function<void()> f);

private:
  std::unordered_map<std::string, std::unique_ptr<PBDev::ParallelTaskConsumer>>
                                   mPTC;
  std::shared_ptr<ProgressService> mProgressService = nullptr;

  std::unordered_map<std::string, PBDev::ProgressId> mProgressNames;
};
} // namespace PB
