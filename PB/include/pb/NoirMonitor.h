#pragma once

#include <chrono>
#include <string>
#include <unordered_map>

#include <pb/Platform.h>

namespace PB {
class NoirMonitor {
public:
  ~NoirMonitor() = default;

  void configurePlatformInfo(std::shared_ptr<PlatformInfo> platformInfo)
  {
    mPlatformInfo = platformInfo;
  }

  void start(const std::string projectId, const std::string &key)
  {
    auto now = std::chrono::high_resolution_clock::now();
    mStartTimes[{projectId, key}] = now;
  }

  void stop(const std::string projectId, const std::string &key)
  {
    auto now = std::chrono::high_resolution_clock::now();
    auto startTime = mStartTimes[{projectId, key}];
    mDurations[{projectId, key}] +=
        std::chrono::duration<double>(now - startTime).count();

    Noir::inst().getLogger()->info("Project: {}, Key: {}, Duration: {} seconds",
                                   projectId, key,
                                   mDurations[{projectId, key}]);
  }

private:
  std::shared_ptr<PlatformInfo> mPlatformInfo = nullptr;
  std::unordered_map<
      std::pair<std::string, std::string>,
      std::chrono::time_point<std::chrono::high_resolution_clock>>
      mStartTimes;

  std::unordered_map<std::pair<std::string, std::string>, double> mDurations;
};
} // namespace PB
