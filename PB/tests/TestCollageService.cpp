#include <gtest/gtest.h>

#include "MockListeners.h"

#include <pb/services/CollageService.h>

using ::testing::_;
using ::testing::AtLeast;
using namespace PB;

TEST(TestCollageService, TestEmpty)
{
  std::shared_ptr<TestCollageMakerListener> testCollageMakerListener =
      std::make_shared<TestCollageMakerListener>();

  std::shared_ptr<TestCollageThumbnailsMakerListener>
      testCollageThumbnailsMakerListener =
          std::make_shared<TestCollageThumbnailsMakerListener>();

  std::shared_ptr<ThreadSchedulerMock> threadScheduler =
      std::make_shared<ThreadSchedulerMock>(std::chrono::milliseconds(5000));

  auto imageFactory = std::make_shared<ImageFactory>();

  std::shared_ptr<TaskCruncher> taskCruncher = std::make_shared<TaskCruncher>();
  taskCruncher->registerPTC("search-files", 1);
  taskCruncher->registerPTC("collage-thumbnails", 1);
  taskCruncher->registerPTC("thumbnails-job", 1);
  taskCruncher->registerPTC("image-search-job", 1);
  taskCruncher->registerPTC("default", 1);

  std::filesystem::path localState =
      std::filesystem::current_path().parent_path() / "test-data" /
      "test-local-state";
  std::filesystem::path installPath =
      std::filesystem::current_path().parent_path() / "test-data" /
      "test-install-folder";

  auto platformInfo = mockPlatformInfo(installPath, localState);

  PB::Project project;

  // string to boost::uuid
  boost::uuids::string_generator gen;
  boost::uuids::uuid projectId = gen("93a24d3d-edce-48fd-a361-b63ce675039d");

  std::shared_ptr<PB::IdentifyableProject> identifyableProject =
      std::make_shared<PB::IdentifyableProject>(
          std::make_pair(projectId, project));

  imageFactory->configurePlatformInfo(platformInfo);

  std::shared_ptr<CollageService> mCollageService =
      std::make_shared<CollageService>();

  mCollageService->configurePlatformInfo(platformInfo);
  mCollageService->configureTaskCruncher(taskCruncher);
  mCollageService->configureProject(identifyableProject);

  mCollageService->configureThumbnailsListener(
      testCollageThumbnailsMakerListener.get());
  mCollageService->configureCollageMakerListener(testCollageMakerListener.get());

  mCollageService->generateTemplatesImages();

  EXPECT_CALL(*testCollageThumbnailsMakerListener, onThumbnailsCreated())
      .Times(1);
  threadScheduler->mainloop();

  auto collagePaths = mCollageService->getTemplatesPaths();

  EXPECT_EQ(collagePaths.size(), 5);

  GenericImagePtr image0 = imageFactory->createRegularImage(
      std::string("1a8ce3aa-eca7-48ff-80a7-805a5db42a34"));

  GenericImagePtr image1 = imageFactory->createRegularImage(
      std::string("1fe051ce-b711-4313-ba42-2946678bf5f4"));

  std::vector<GenericImagePtr> images = {image0, image1};

  EXPECT_CALL(*testCollageMakerListener, onCollageCreated(_))
      .Times(1);

  mCollageService->combineImages(0, images);



  threadScheduler->mainloop();
}
