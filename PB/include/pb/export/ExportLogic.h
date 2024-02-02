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

  template <typename T>
  void start(std::stop_source& stopSource, T task)
  {
    std::get<PBDev::SequentialTaskConsumer<T>>(mExporters)
        .configure(stopSource.get_token());

    std::get<PBDev::SequentialTaskConsumer<T>>(mExporters)
        .configure(task);

    auto listener =
        std::get<PBDev::SequentialTaskConsumerListener<T> *>(mListeners);

    std::get<PBDev::SequentialTaskConsumer<T>>(mExporters).configure(listener);

    std::get<PBDev::SequentialTaskConsumer<T>>(mExporters).start();
  }

private:

  std::tuple<PBDev::SequentialTaskConsumer<ExporterTypes>...> mExporters;
  std::tuple<PBDev::SequentialTaskConsumerListener<ExporterTypes> *...>
                                    mListeners;
  std::shared_ptr<PB::Project>      mProject;
  std::shared_ptr<PB::PlatformInfo> mPlatformInfo;

  std::vector<std::shared_ptr<VirtualImage>> mPtrImages;
};
} // namespace PB