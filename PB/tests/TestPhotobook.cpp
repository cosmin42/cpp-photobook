#include "MockListeners.h"

#include <pb/components/ProjectMetadata.h>

TEST(TestPhotobook, TestCreation)
{
  std::shared_ptr<PB::StagedImagesListener> stagedImageListener =
      std::make_shared<TestPhotobookStagedImagesListener>();

  std::shared_ptr<PB::ImageMonitorListener> imageMonitorListener =
      std::make_shared<MockPhotobookImageMonitorListener>();

  PB::Photobook photobook(".", ".", {1280, 720});
  photobook.configure(stagedImageListener.get());
  photobook.configure(imageMonitorListener.get());
}
/*
TEST(TestPhotobook, TestMetadata)
{
  clearProjectCache();

  TestPhotobookListener testPhotobookListener;

  std::shared_ptr<PB::StagedImagesListener> stagedImageListener =
      std::make_shared<TestPhotobookStagedImagesListener>();

  std::shared_ptr<PB::ImageMonitorListener> imageMonitorListener =
      std::make_shared<MockPhotobookImageMonitorListener>();

  PB::Photobook photobook(".", ".", {1280, 720});
  photobook.configure(stagedImageListener.get());
  photobook.configure(imageMonitorListener.get());
  photobook.configure(&testPhotobookListener);

  EXPECT_CALL(testPhotobookListener, onMetadataUpdated());

  photobook.recallMetadata();

  std::vector<PB::ProjectMetadata> projectsMetadata;

  for (int i = 0; i < 4; ++i) {
    std::string name = "random-name" + std::to_string(i);
    photobook.newProject(name, PB::PaperSettings());

    auto uuid = photobook.project()->currentProjectUUID();
    auto path = Path(".") / name;

    projectsMetadata.push_back(
        PB::ProjectMetadata{boost::uuids::to_string(uuid), path.string()});

    EXPECT_CALL(testPhotobookListener, onMetadataUpdated());

    photobook.recallMetadata();
  }
  std::this_thread::sleep_for(std::chrono::milliseconds(3000));

  for (int i = 0; (int)i < 4; ++i) {
    photobook.project()->remove(projectsMetadata.at(0).uuid());
    projectsMetadata.erase(projectsMetadata.begin());

    std::this_thread::sleep_for(std::chrono::milliseconds(3000));

    EXPECT_CALL(testPhotobookListener, onMetadataUpdated());

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

  TestPhotobookListener testPhotobookListener;

  PB::Photobook photobook(".", ".", {1280, 720});
  photobook.configure(stagedImageListener.get());
  photobook.configure(imageMonitorListener.get());
  photobook.configure(&testPhotobookListener);

  EXPECT_CALL(testPhotobookListener, onMetadataUpdated());

  photobook.recallMetadata();

  photobook.newProject("random-name", PB::PaperSettings());

  auto uuid = photobook.project()->currentProjectUUID();
  auto projectPath = Path(".") / "random-name.photobook";

  EXPECT_CALL(testPhotobookListener, onProjectRead());
  photobook.recallProject("random-name");
}

TEST(TestPhotobook, TestProjectLoading)
{
  clearProjectCache();

  std::shared_ptr<PB::StagedImagesListener> stagedImageListener =
      std::make_shared<TestPhotobookStagedImagesListener>();

  std::shared_ptr<PB::ImageMonitorListener> imageMonitorListener =
      std::make_shared<MockPhotobookImageMonitorListener>();

  TestPhotobookListener testPhotobookListener;

  PB::Photobook photobook(".", ".", {1280, 720});
  photobook.configure(stagedImageListener.get());
  photobook.configure(imageMonitorListener.get());
  photobook.configure((PB::PhotobookListener *)&testPhotobookListener);

  EXPECT_CALL(testPhotobookListener, onMetadataUpdated());

  photobook.recallMetadata();

  photobook.newProject("random-name", PB::PaperSettings());

  auto uuid = photobook.project()->currentProjectUUID();
  auto projectPath = Path(".") / "random-name.photobook";

  EXPECT_CALL(testPhotobookListener, onProjectRead());
  photobook.recallProject("random-name");

  std::this_thread::sleep_for(std::chrono::milliseconds(3000));

  EXPECT_CALL(*(MockPhotobookImageMonitorListener *)imageMonitorListener.get(),
              onCleared());
  photobook.unloadProject();
}
*/