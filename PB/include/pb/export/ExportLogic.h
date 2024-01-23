#pragma once

#include <vector>

#include <pb/export/PdfPoDoFo.h>
#include <pb/tasks/SequentialTaskConsumer.h>

namespace PB {

template <typename T> class ExportLogicListener {
public:
};

template <typename... ExporterTypes> class ExportLogic final {
public:
  template <typename T> T const &exporter() const
  {
    return std::get<T>(mExporters);
  }

  void configure(std::shared_ptr<PB::Project>      project,
                 std::shared_ptr<PB::PlatformInfo> platformInfo)
  {
    mProject = project;
    mPlatformInfo = platformInfo;
  }

  template <typename T>
  void configure(PBDev::SequentialTaskConsumerListener<T> *listener)
  {
    std::get<PBDev::SequentialTaskConsumerListener<T> *>(mListeners) = listener;
  }

  void configure(std::vector<std::shared_ptr<VirtualImage>> const &stagedImages)
  {
    for (auto &it : stagedImages) {
      mPtrImages.push_back(it);
    }
  }

  void start(std::stop_source &stopSource)
  {
    start<ExporterTypes...>(stopSource, stopSource.get_token());
  }

private:
  template <typename T> T makeTask()
  {
    T task{Path(), mPlatformInfo->localStatePath, mProject->active().paperSettings,
           mPtrImages};
    return task;
  }

  template <typename T>
  void start(std::stop_source &stopSource, std::stop_token stopToken)
  {
    std::get<PBDev::SequentialTaskConsumer<T>>(mExporters).configure(stopToken);

    std::get<PBDev::SequentialTaskConsumer<T>>(mExporters)
        .configure(makeTask<T>());

    auto listener =
        std::get<PBDev::SequentialTaskConsumerListener<T> *>(mListeners);

    std::get<PBDev::SequentialTaskConsumer<T>>(mExporters).configure(listener);

    std::get<PBDev::SequentialTaskConsumer<T>>(mExporters).start();
  }
  /*
  template <typename Head, typename... Tail>
  void start(std::stop_source &stopSource, std::stop_token stopToken)
  {
    start<Head>(stopToken);
    start<Tail...>(stopSource, stopSource.get_token());
  }*/

  std::tuple<PBDev::SequentialTaskConsumer<ExporterTypes>...> mExporters;
  std::tuple<PBDev::SequentialTaskConsumerListener<ExporterTypes> *...>
                                    mListeners;
  std::shared_ptr<PB::Project>      mProject;
  std::shared_ptr<PB::PlatformInfo> mPlatformInfo;

  std::vector<std::shared_ptr<VirtualImage>> mPtrImages;
};
} // namespace PB