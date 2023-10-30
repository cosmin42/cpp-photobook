#pragma once

#include <string>

#include <pb/util/Thread.h>
#include <pb/util/Traits.h>
#include <pb/PaperSettings.h>

namespace PB {
class Exportable : public Thread {
public:
  Exportable(std::stop_token stopToken, PaperSettings paperSettings,
             Path temporaryDirectory)
      : Thread(stopToken), mPaperSettings(paperSettings),
        mTemporaryDirectory(temporaryDirectory)
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

protected:
  std::string       mName;
  Path              mDestination;
  std::vector<Path> mImages;
  PaperSettings     mPaperSettings;
  Path              mTemporaryDirectory;
};
} // namespace PB