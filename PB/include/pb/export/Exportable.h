#pragma once

#include <string>

#include <pb/project/PaperSettings.h>
#include <pb/tasks/SequentialTaskConsumer.h>
#include <pb/util/Util.h>

namespace PB {
class Exportable : public PBDev::ObservableSubject {
public:
  Exportable(std::stop_token stopToken, PaperSettings paperSettings,
             Path temporaryDirectory)
      : mPaperSettings(paperSettings), mTemporaryDirectory(temporaryDirectory)
  {
  }

  virtual ~Exportable() = default;

  void configureExport(std::string name, Path destination,
                       std::vector<Path> images)
  {
    mName = name;
    mDestination = destination;
    for (auto &image : images) {
      mImages.push_back(image);
    }
  }

  void notify() override
  {
    for (auto observer : mObservers) {
      observer->update(*this);
    }
  }

protected:
  std::string       mName;
  Path              mDestination;
  std::vector<Path> mImages;
  PaperSettings     mPaperSettings;
  Path              mTemporaryDirectory;
};
} // namespace PB