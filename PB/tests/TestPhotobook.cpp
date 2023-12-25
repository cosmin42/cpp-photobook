#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <regex>

#include <pb/PhotoBook.h>

class TestPhotobookStagedImagesListener : public PB::StagedImagesListener {
public:
  MOCK_METHOD(void, onPicturesAdded, (int index, int size), (override));
  MOCK_METHOD(void, onPictureRemoved, (std::vector<unsigned> index),
              (override));
};

class MockPhotobookImageMonitorListener final
    : public PB::ImageMonitorListener {
public:
  MOCK_METHOD(void, onImportFolderAdded, (), (override));
  MOCK_METHOD(void, onImportFolderRemoved, (unsigned index), (override));
  MOCK_METHOD(void, onCleared, (), (override));
};

class TestDashboardListener final : public PB::DashboardListener {
public:
  MOCK_METHOD(void, onProjectsMetadataLoaded,
              (std::vector<PB::ProjectMetadata> v), (override));
  MOCK_METHOD(void, onProjectRead, (), (override));
};

void clearProjectCache()
{
  std::regex uuidRegex = std::regex(
      "[0-9a-f]{8}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{12}");

  std::filesystem::remove("database.db");

  for (const auto &entry : std::filesystem::directory_iterator(".")) {
    if (std::regex_match(entry.path().stem().string(), uuidRegex)) {
      std::filesystem::remove(entry.path());
    }
  }
}

TEST(TestPhotobook, TestCreation)
{
  std::shared_ptr<PB::StagedImagesListener> stagedImageListener =
      std::make_shared<TestPhotobookStagedImagesListener>();

  std::shared_ptr<PB::ImageMonitorListener> imageMonitorListener =
      std::make_shared<MockPhotobookImageMonitorListener>();

  PB::Photobook photobook(".");
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

  PB::Photobook photobook(".");
  photobook.configure(stagedImageListener.get());
  photobook.configure(imageMonitorListener.get());

  TestDashboardListener testDashboardListener;

  photobook.configure((PB::DashboardListener *)&testDashboardListener);

  EXPECT_CALL(testDashboardListener,
              onProjectsMetadataLoaded(std::vector<PB::ProjectMetadata>()));

  photobook.recallMetadata();

  std::vector<PB::ProjectMetadata> projectsMetadata;

  for (int i = 0; i < 4; ++i) {
    photobook.newProject();

    auto uuid = photobook.activeProject().uuid();
    auto path = photobook.activeProject().parentDirectory() /
                (boost::uuids::to_string(uuid) + ".photobook");

    projectsMetadata.push_back(
        PB::ProjectMetadata{boost::uuids::to_string(uuid), path.string()});

    EXPECT_CALL(testDashboardListener,
                onProjectsMetadataLoaded(projectsMetadata));

    photobook.recallMetadata();
  }
  Sleep(3000);

  for (int i = 0; (int)i < 4; ++i) {
    photobook.deleteProject(
        boost::uuids::to_string(projectsMetadata.at(0).uuid()));
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

  PB::Photobook photobook(".");
  photobook.configure(stagedImageListener.get());
  photobook.configure(imageMonitorListener.get());

  TestDashboardListener testDashboardListener;

  photobook.configure((PB::DashboardListener *)&testDashboardListener);

  EXPECT_CALL(testDashboardListener,
              onProjectsMetadataLoaded(std::vector<PB::ProjectMetadata>()));

  photobook.recallMetadata();

  std::vector<PB::ProjectMetadata> projectsMetadata;

  photobook.newProject();

  auto uuid = photobook.activeProject().uuid();
  auto projectPath = photobook.activeProject().parentDirectory() /
                     (boost::uuids::to_string(uuid) + ".photobook");

  EXPECT_CALL(testDashboardListener,
              onProjectRead());
  photobook.recallProject(projectPath);
}

TEST(TestPhotobook, TestProjectLoading)
{
  clearProjectCache();

  std::shared_ptr<PB::StagedImagesListener> stagedImageListener =
      std::make_shared<TestPhotobookStagedImagesListener>();

  std::shared_ptr<PB::ImageMonitorListener> imageMonitorListener =
      std::make_shared<MockPhotobookImageMonitorListener>();

  PB::Photobook photobook(".");
  photobook.configure(stagedImageListener.get());
  photobook.configure(imageMonitorListener.get());

  TestDashboardListener testDashboardListener;

  photobook.configure((PB::DashboardListener *)&testDashboardListener);

  EXPECT_CALL(testDashboardListener,
              onProjectsMetadataLoaded(std::vector<PB::ProjectMetadata>()));

  photobook.recallMetadata();

  std::vector<PB::ProjectMetadata> projectsMetadata;

  photobook.newProject();

  auto uuid = photobook.activeProject().uuid();
  auto projectPath = photobook.activeProject().parentDirectory() /
                     (boost::uuids::to_string(uuid) + ".photobook");

  EXPECT_CALL(testDashboardListener, onProjectRead());
  photobook.recallProject(projectPath);
}

TEST(TestPhotobook, TestProjectLoading)
{
  clearProjectCache();

  std::shared_ptr<PB::StagedImagesListener> stagedImageListener =
      std::make_shared<TestPhotobookStagedImagesListener>();

  std::shared_ptr<PB::ImageMonitorListener> imageMonitorListener =
      std::make_shared<MockPhotobookImageMonitorListener>();

  PB::Photobook photobook(".");
  photobook.configure(stagedImageListener.get());
  photobook.configure(imageMonitorListener.get());

  TestDashboardListener testDashboardListener;

  photobook.configure((PB::DashboardListener *)&testDashboardListener);

  EXPECT_CALL(testDashboardListener,
              onProjectsMetadataLoaded(std::vector<PB::ProjectMetadata>()));

  photobook.recallMetadata();

  std::vector<PB::ProjectMetadata> projectsMetadata;

  photobook.newProject();

  auto uuid = photobook.activeProject().uuid();
  auto projectPath = photobook.activeProject().parentDirectory() /
                     (boost::uuids::to_string(uuid) + ".photobook");

  EXPECT_CALL(testDashboardListener, onProjectRead());
  photobook.recallProject(projectPath);

  Sleep(3000);

  photobook.loadProject();

  photobook.unloadProject();
}