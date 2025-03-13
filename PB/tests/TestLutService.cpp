#include <gtest/gtest.h>

#include "MockListeners.h"

#include <pb/services/LutService.h>

using ::testing::_;
using ::testing::AtLeast;
using namespace PB;

TEST(TestLutService, TestEmpty)
{
  ProgressServiceListenerTest *progressServiceListener =
      new ProgressServiceListenerTest();

  TestLutServiceListener *testLutServiceListener = new TestLutServiceListener();

  ThreadSchedulerMock *threadScheduler =
      new ThreadSchedulerMock(std::chrono::milliseconds(60000));

  auto vulkanManager = std::make_shared<PB::VulkanManager>();

  std::shared_ptr<ProgressService> progressService =
      std::make_shared<ProgressService>();
  progressService->configure(progressServiceListener);
  progressService->configureScheduler(threadScheduler);

  std::shared_ptr<TaskCruncher> taskCruncher = std::make_shared<TaskCruncher>();
  taskCruncher->configureProgressService(progressService);

  taskCruncher->registerPTC("search-files", 1);
  taskCruncher->registerPTC("lut-icons", 1);
  taskCruncher->registerPTC("thumbnails-job", 1);
  taskCruncher->registerPTC("default", 1);

  auto imageFactory = std::make_shared<ImageFactory>();

  std::filesystem::path localState =
      std::filesystem::current_path().parent_path() / "test-data" /
      "test-local-state";
  std::filesystem::path installPath =
      std::filesystem::current_path().parent_path() / "test-data" /
      "test-install-folder";

  auto platformInfo = mockPlatformInfo(installPath, localState);

  vulkanManager->init();

  PB::Project project;

  // string to boost::uuid
  boost::uuids::string_generator gen;
  boost::uuids::uuid projectId = gen("93a24d3d-edce-48fd-a361-b63ce675039d");

  std::shared_ptr<PB::IdentifyableProject> identifyableProject =
      std::make_shared<PB::IdentifyableProject>(
          std::make_pair(projectId, project));

  imageFactory->configurePlatformInfo(platformInfo);

  std::shared_ptr<OGLEngine> mOGLEngine = std::make_shared<OGLEngine>();
  mOGLEngine->configurePlatformInfo(platformInfo);
  mOGLEngine->configVulkanManager(vulkanManager);

  std::shared_ptr<LutService> mLutService = std::make_shared<LutService>();
  mLutService->configurePlatformInfo(platformInfo);
  mLutService->configureTaskCruncher(taskCruncher);
  mLutService->configureOGLEngine(mOGLEngine);
  mLutService->configureThreadScheduler(threadScheduler);
  mLutService->configureLutServiceListener(testLutServiceListener);
  mLutService->configureDurableHashService(
      mockDurableHashService(mockDatabaseService(platformInfo)));
  mLutService->configureProject(identifyableProject);
  mLutService->configureImageFactory(imageFactory);

  EXPECT_CALL(*progressServiceListener, progressUpdate(_)).Times(AtLeast(25));
  EXPECT_CALL(*testLutServiceListener, onLutAdded(_)).Times(AtLeast(25));
  EXPECT_CALL(*testLutServiceListener, onLutApplied(_, _)).Times(AtLeast(1));

  mOGLEngine->start();
  mLutService->startLutService();

  threadScheduler->mainloop();

  // count files in folder
  auto count = std::count_if(
      std::filesystem::directory_iterator(platformInfo->localStatePath /
                                          "processed-luts"),
      std::filesystem::directory_iterator(), [](auto) { return true; });

  EXPECT_EQ(count, 25);
  std::filesystem::remove_all(platformInfo->processedLutsPath());

  GenericImagePtr image0 = imageFactory->createRegularImage(
      std::string("1a8ce3aa-eca7-48ff-80a7-805a5db42a34"));

  mLutService->applyTransformationOnDisk(
      PBDev::LutApplicationId(RuntimeUUID::newUUID()), 7, image0, 1.0, 1.0,
      0.0);

  // mLutService->applyLut(PBDev::LutApplicationId(RuntimeUUID::newUUID()), 7,
  //                       image0);

  //  threadScheduler->mainloop();

  delete threadScheduler;
  delete testLutServiceListener;
  delete progressServiceListener;
}