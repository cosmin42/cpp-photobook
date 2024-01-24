#pragma once

#include <string>
#include <unordered_map>

#include <pb/Enums.h>

#include <pb/Config.h>

namespace PB {

struct ProgressInfo {
  JobType      jobType = JobType::None;
  ProgressType progressType;
  int          progress = 0;
  int          progressCap = 0;
};

class ProgressManagerListener {
public:
  virtual void progressUpdate(std::vector<std::string> names,
                              ProgressInfo             definedProgress,
                              ProgressInfo             undefinedProgress) = 0;
};

class ProgressManager final {
public:
  ~ProgressManager() = default;

  void configure(ProgressManagerListener *listener);

  void subscribe(std::string name, JobType jobType, int progressCap = 0);
  void update(std::string name);
  void abort(std::string name);
  void finish(std::string name);

private:
  ProgressInfo totalDefiniteProgress() const;
  ProgressInfo totalIndefiniteProgress() const;

  std::vector<std::string> names() const;

  ProgressManagerListener *mListener = nullptr;

  std::unordered_map<std::string, ProgressInfo> mProgress;
};
} // namespace PB