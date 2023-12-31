#pragma once

#include <stop_token>
#include <thread>

#include <pb/Config.h>
#include <pb/util/Util.h>

namespace PBDev {

template <PB::TaskConcept Task> class SequentialTaskConsumerListener {
public:
  virtual ~SequentialTaskConsumerListener() = default;
  virtual void started(Task const &task) = 0;
  virtual void finished(Task const &task) = 0;
  virtual void aborted(Task const &task) = 0;
};

template <PB::TaskConcept Task> class SequentialTaskConsumer final {
public:
  void configure(SequentialTaskConsumerListener<Task> *listener)
  {
    mListener = listener;
  }

  void configure(std::stop_token stopToken) { mExternalToken = stopToken; }
  void configure(Task task) { mTask = task; }

  void start()
  {
    mThread = std::jthread([this](std::stop_token token) { run(token); });
  }

  Task task() const { return mTask.value(); }

private:
  void run(std::stop_token token)
  {
    PBDev::Timer timer;
    mCurrentToken = token;
    mListener->started(mTask.value());
    while (!mCurrentToken.stop_requested() &&
           !mExternalToken.stop_requested()) {
      if (mTask->stoppingCondition()) {
        mThread.request_stop();
      }
      else {
        mTask->taskStep();
      }
    }
    if (mCurrentToken.stop_requested()) {
      mListener->finished(mTask.value());
    }
    else {
      mListener->aborted(mTask.value());
    }
  }

  SequentialTaskConsumerListener<Task> *mListener = nullptr;

  std::stop_token     mCurrentToken;
  std::stop_token     mExternalToken;
  std::jthread        mThread;
  std::optional<Task> mTask;
};
} // namespace PBDev