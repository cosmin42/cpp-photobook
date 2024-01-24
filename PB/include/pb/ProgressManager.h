#pragma once

#include <string>
#include <unordered_map>

#include <pb/Enums.h>

namespace PB {

struct ProgressInfo {
  JobType jobType = JobType::None;
  int     progress = 0;
  int     progressCap = 0;
};

class ProgressManagerListener {
public:
  virtual void progressStarted(std::string name, ProgressInfo progressInfo) = 0;
  virtual void progressUpdate(std::string name, ProgressInfo progressInfo) = 0;
  virtual void progressAborted(std::string name) = 0;
};

class ProgressManager final {
public:
  ~ProgressManager() = default;

  void configure(ProgressManagerListener *listener);

  void subscribe(std::string name, JobType jobType, int progressCap = 0);
  void update(std::string name);
  void abort(std::string name);

private:
  ProgressManagerListener *mListener = nullptr;

  std::unordered_map<std::string, ProgressInfo> mProgress;
};
} // namespace PB