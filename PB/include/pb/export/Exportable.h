#pragma once

#include <string>

#include <pb/project/PaperSettings.h>
#include <pb/tasks/SequentialTaskConsumer.h>
#include <pb/util/Util.h>

namespace PB {
enum class ExportableState { None, Started, Finished, Stopped };

class Exportable {
public:
  Exportable(std::stop_token stopToken, PaperSettings paperSettings,
             Path temporaryDirectory)
      //: mPaperSettings(paperSettings), mTemporaryDirectory(temporaryDirectory)
  {
  }

  virtual ~Exportable() = default;
  /*
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

  ExportableState state() const { return mState; }

protected:
  std::string       mName;
  Path              mDestination;
  std::vector<Path> mImages;
  PaperSettings     mPaperSettings;
  Path              mTemporaryDirectory;

  ExportableState mState = ExportableState::None;
  */
};
} // namespace PB