#include <gtest/gtest.h>

#include "MockListeners.h"

#include <pb/services/EffectsService.h>

using ::testing::_;
using ::testing::AtLeast;
using namespace PB;

TEST(TestEffectsService, TestEmpty)
{
  ProgressServiceListenerTest *progressServiceListener =
      new ProgressServiceListenerTest();

  TestEffectsServiceListener *testEffectsServiceListener =
      new TestEffectsServiceListener();

  ThreadSchedulerMock *threadScheduler =
      new ThreadSchedulerMock(std::chrono::milliseconds(10000));

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


  std::shared_ptr<EffectsService> effectsService =
      std::make_shared<EffectsService>();

  effectsService->configureEffectsServiceListener(testEffectsServiceListener);
  effectsService->configurePlatformInfo(platformInfo);
  effectsService->configureTaskCruncher(taskCruncher);
  effectsService->configureImageFactory(imageFactory);
  effectsService->configureProject(identifyableProject);

  GenericImagePtr image0 = imageFactory->createRegularImage(
      std::string("1a8ce3aa-eca7-48ff-80a7-805a5db42a34"));

  effectsService->applySaturation(PBDev::EffectId(RuntimeUUID::newUUID()),
                                  image0, 1.8, false);

  threadScheduler->mainloop();

  delete threadScheduler;
  delete testEffectsServiceListener;
  delete progressServiceListener;
}