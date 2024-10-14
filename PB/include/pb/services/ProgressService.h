#pragma once

#include <string>
#include <unordered_map>

#include <pb/Config.h>
#include <pb/Enums.h>
#include <pb/components/ThreadScheduler.h>
#include <pb/util/Traits.h>

DECLARE_STRONG_UUID(ProgressId)
DECLARE_STRONG_STRING(ProgressJobName)

namespace PB {
struct ProgressInfo {
  PBDev::ProgressJobName name;
  unsigned               progress = 0;
  unsigned               progressCap = 0;
};

struct ProgressStatus {
  std::unordered_map<PBDev::ProgressJobName, std::optional<float>,
                     boost::hash<PBDev::ProgressJobName>>
      statusMap;

  std::string toString()
  {
    if (statusMap.empty()) {
      return "Idle.";
    }
    std::string knownProgress;
    std::string unknownProgress;
    for (auto const &[name, maybePercent] : statusMap) {
      if (maybePercent) {
        knownProgress = *name + ": " + std::to_string(*maybePercent) + ";";
      }
      else {
        unknownProgress = *name + ";";
      }
    }

    return "Pending: {" + knownProgress + "}, {" + unknownProgress + "}";
  }
};
} // namespace PB

namespace PB::Service {

class ProgressServiceListener {
public:
  virtual void progressUpdate(PB::ProgressStatus status) = 0;
};

class ProgressService final {
public:
  ~ProgressService() = default;

  void configure(ProgressServiceListener *listener);
  void configureScheduler(PBDev::ThreadScheduler *scheduler);

  PBDev::ProgressId start(PBDev::ProgressJobName name, unsigned taskCount = 0);
  void              update(PBDev::ProgressId id);
  void              finish(PBDev::ProgressId id);
  void              abortAll();

private:
  ProgressServiceListener *mListener = nullptr;
  PBDev::ThreadScheduler  *mScheduler = nullptr;

  std::unordered_map<PBDev::ProgressId, ProgressInfo,
                     boost::hash<PBDev::ProgressId>>
      mProgressData;

  ProgressStatus mProgressStatus;

  ProgressStatus aggregateStatus() const;
};
} // namespace PB::Service