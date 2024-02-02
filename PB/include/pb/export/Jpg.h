#pragma once

#include <pb/project/Project.h>
#include <pb/util/Traits.h>

namespace PB {
class JpgExport final {
public:
  explicit JpgExport(
      Path root, PaperSettings paperSettings,
      std::vector<std::shared_ptr<VirtualImage>> const &stagedImages);
  ~JpgExport() = default;

  bool stoppingCondition() const;
  int  stepsCount() const;
  void taskStep();

private:
  struct JPG_TEMPLATE_PARAMS {
    static constexpr const char *JPG_NAME_TEMPLATE = "PHOTO_0000000.JPG";
    static constexpr unsigned    PLACEHOLDER_SIZE = 7;
    static constexpr unsigned    PLACEHOLDER_OFFSET = 6;
  };

  std::string makeName(unsigned counter);
  void        writeImage(Path inputPath, Path outputPath) const;

  unsigned                                   mIndex = 0;
  Path                                       mRoot;
  PaperSettings                              mPaperSettings;
  std::vector<std::shared_ptr<VirtualImage>> mStagedImages;
};
} // namespace PB