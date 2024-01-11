#include "MockListeners.h"

TEST(TestPhotobook, TestCreation)
{
  std::shared_ptr<PB::StagedImagesListener> stagedImageListener =
      std::make_shared<TestPhotobookStagedImagesListener>();

  std::shared_ptr<PB::ImageMonitorListener> imageMonitorListener =
      std::make_shared<MockPhotobookImageMonitorListener>();

  PB::Photobook photobook(".", ".");
  photobook.configure(stagedImageListener.get());
  photobook.configure(imageMonitorListener.get());
}

TEST(TestPhotobook, TestMetadata)
{
  clearProjectCache();

  std::shared_ptr<PB::StagedImagesListener> stagedImageListener =
      std::make_shared<TestPhotobookStagedImagesListener>();

  std::shared_ptr<PB::ImageMonitorListener> imageMonitorListener =
      std::make_shared<MockPhotobookImageMonitorListener>();

  PB::Photobook photobook(".", ".");
  photobook.configure(stagedImageListener.get());
  photobook.configure(imageMonitorListener.get());

  TestDashboardListener testDashboardListener;

  photobook.configure((PB::DashboardListener *)&testDashboardListener);

  EXPECT_CALL(testDashboardListener,
              onProjectsMetadataLoaded(std::vector<PB::ProjectMetadata>()));

  photobook.recallMetadata();

  std::vector<PB::ProjectMetadata> projectsMetadata;

  for (int i = 0; i < 4; ++i) {
    photobook.newProject("random-name");

    auto uuid = photobook.project().currentProject()->active().uuid;
    auto path = PB::ProjectSnapshot::parentDirectory() /
                (boost::uuids::to_string(uuid) + ".photobook");

    projectsMetadata.push_back(
        PB::ProjectMetadata{boost::uuids::to_string(uuid), path.string()});

    EXPECT_CALL(testDashboardListener,
                onProjectsMetadataLoaded(projectsMetadata));

    photobook.recallMetadata();
  }
  Sleep(3000);

  for (int i = 0; (int)i < 4; ++i) {
    photobook.project().remove(projectsMetadata.at(0).uuid());
    projectsMetadata.erase(projectsMetadata.begin());

    Sleep(3000);

    EXPECT_CALL(testDashboardListener,
                onProjectsMetadataLoaded(projectsMetadata));

    photobook.recallMetadata();
  }
}

TEST(TestPhotobook, TestProject)
{
  clearProjectCache();

  std::shared_ptr<PB::StagedImagesListener> stagedImageListener =
      std::make_shared<TestPhotobookStagedImagesListener>();

  std::shared_ptr<PB::ImageMonitorListener> imageMonitorListener =
      std::make_shared<MockPhotobookImageMonitorListener>();

  PB::Photobook photobook(".", ".");
  photobook.configure(stagedImageListener.get());
  photobook.configure(imageMonitorListener.get());

  TestDashboardListener testDashboardListener;

  photobook.configure((PB::DashboardListener *)&testDashboardListener);

  EXPECT_CALL(testDashboardListener,
              onProjectsMetadataLoaded(std::vector<PB::ProjectMetadata>()));

  photobook.recallMetadata();

  std::vector<PB::ProjectMetadata> projectsMetadata;

  photobook.newProject("random-name");

  auto uuid = photobook.project().currentProject()->active().uuid;
  auto projectPath = PB::ProjectSnapshot::parentDirectory() /
                     (boost::uuids::to_string(uuid) + ".photobook");

  EXPECT_CALL(testDashboardListener, onProjectRead());
  photobook.recallProject("random-name");
}

TEST(TestPhotobook, TestProjectLoading)
{
  clearProjectCache();

  std::shared_ptr<PB::StagedImagesListener> stagedImageListener =
      std::make_shared<TestPhotobookStagedImagesListener>();

  std::shared_ptr<PB::ImageMonitorListener> imageMonitorListener =
      std::make_shared<MockPhotobookImageMonitorListener>();

  TestPhotobookListener photobookListener;

  PB::Photobook photobook(".", ".");
  photobook.configure(stagedImageListener.get());
  photobook.configure(imageMonitorListener.get());

  TestDashboardListener testDashboardListener;

  photobook.configure((PB::DashboardListener *)&testDashboardListener);
  photobook.configure((PB::PhotobookListener *)&photobookListener);

  EXPECT_CALL(testDashboardListener,
              onProjectsMetadataLoaded(std::vector<PB::ProjectMetadata>()));

  photobook.recallMetadata();

  std::vector<PB::ProjectMetadata> projectsMetadata;

  photobook.newProject("random-name");

  auto uuid = photobook.project().currentProject()->active().uuid;
  auto projectPath = PB::ProjectSnapshot::parentDirectory() /
                     (boost::uuids::to_string(uuid) + ".photobook");

  EXPECT_CALL(testDashboardListener, onProjectRead());
  photobook.recallProject("random-name");

  Sleep(3000);

  photobook.loadProject();

  photobook.unloadProject();
}
