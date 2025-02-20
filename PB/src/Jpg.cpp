#include <pb/export/Jpg.h>

#include <pb/image/ImageOperations.h>
#include <pb/image/ImageReader.h>

#include <string>

namespace PB {

JpgExport::JpgExport(Path root, PaperSettings paperSettings,
                     std::vector<GenericImagePtr> const &stagedImages)
    : mRoot(root), mPaperSettings{paperSettings}
{
  for (auto const &it : stagedImages) {
    mStagedImages.push_back(it);
  }
}

bool JpgExport::stoppingCondition() const
{
  return mIndex >= mStagedImages.size();
}

int JpgExport::stepsCount() const { return (int)mStagedImages.size(); }

void JpgExport::taskStep()
{
  Path tmpImage = mRoot / makeName(mIndex);

  auto virtualImage = mStagedImages.at(mIndex);
  auto imagePath = mPlatformInfo->thumbnailByHash(mProject->first,
                                                  virtualImage->hash(), ".jpg");
  writeImage(imagePath, tmpImage);
  mIndex++;

  if constexpr (OneConfig::SIMULATE_SLOW_EXPORTER) {
    std::this_thread::sleep_for(std::chrono::milliseconds(3000));
  }
}

std::string JpgExport::name() const
{
  return mRoot.parent_path().filename().string();
}

void JpgExport::writeImage(Path inputPath, Path outputPath) const
{
  std::shared_ptr<cv::Mat> image = PB::Process::singleColorImage(
      mPaperSettings.width, mPaperSettings.height, {255, 255, 255})();

  auto temporaryImage = ImageReader().read(inputPath, true);
  PBDev::basicAssert(temporaryImage != nullptr);
  Process::resize(temporaryImage, {mPaperSettings.width, mPaperSettings.height},
                  true);
  PB::Process::overlap(temporaryImage, PB::Process::alignToCenter())(image);

  bool success = cv::imwrite(outputPath.string(), *image);
  PBDev::basicAssert(success);
}

std::string JpgExport::makeName(unsigned counter)
{
  auto counterStr = std::to_string(counter);
  PBDev::basicAssert(counterStr.size() <=
                     JPG_TEMPLATE_PARAMS::PLACEHOLDER_SIZE);
  std::string result = JPG_TEMPLATE_PARAMS::JPG_NAME_TEMPLATE;
  unsigned    replaceOffset =
      JPG_TEMPLATE_PARAMS::PLACEHOLDER_OFFSET +
      (JPG_TEMPLATE_PARAMS::PLACEHOLDER_SIZE - (unsigned)counterStr.length());
  result.replace(replaceOffset, counterStr.size(), counterStr);

  return result;
}

std::optional<IdentifyableFunction>
JpgExport::getTask(std::stop_token stopToken)
{
  if (mCrunchedFlag) {
    return std::nullopt;
  }

  mStopToken = stopToken;

  IdentifyableFunction f{
      RuntimeUUID::newUUID(), [this, stopToken{stopToken}]() {
        while (!stoppingCondition() && !stopToken.stop_requested()) {
          taskStep();
          mListener->onExportUpdate(mId);
        }
      }};
  mCrunchedFlag = true;
  return f;
}

void JpgExport::onTaskFinished(PBDev::MapReducerTaskId id)
{
  if (mStopToken.stop_requested()) {
    mListener->onExportAborted(id);
  }
  else {
    mListener->onExportComplete(id);
  }
}

} // namespace PB
