#pragma once

#include <pb/MapReducer.h>

namespace PB {
class LutIconsPreprocessingListener {
public:
  virtual ~LutIconsPreprocessingListener() = default;

  virtual void onLutIconsPreprocessingFinished(Path) = 0;
};

class LutIconsPreprocessingJob final : public MapReducer {
public:
  LutIconsPreprocessingJob(std::vector<Path> const &luts) : mLuts(luts) {}
  ~LutIconsPreprocessingJob() = default;

  std::optional<IdentifyableFunction>
  getTask(std::stop_token stopToken) override
  {
    if (mIndex >= mLuts.size()) {
      return std::nullopt;
    }
    auto lutPath = mLuts.at(mIndex);

    PBDev::MapReducerTaskId taskId(RuntimeUUID::newUUID());

    return std::make_optional<IdentifyableFunction>(taskId,
                                                    [this, taskId, lutPath] {
                                                      // Do some work with
                                                      // lutPath
                                                      // ...
                                                      onTaskFinished(taskId);
                                                    });
  }

  void onTaskFinished(PBDev::MapReducerTaskId) override {}

  unsigned taskCount() const override { return (unsigned)mLuts.size(); }

private:
  std::vector<Path> mLuts;
  std::vector<Path> mIcons;
  unsigned          mIndex = 0;
};
} // namespace PB
