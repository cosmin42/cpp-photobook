#include "MockListeners.h"

using ::testing::_;
using ::testing::AtLeast;

TEST(TestFolderImport, Test0)
{
  clearProjectCache();

  std::shared_ptr<PB::StagedImagesListener> stagedImageListener =
      std::make_shared<TestPhotobookStagedImagesListener>();

  MockPhotobookImageMonitorListener imageMonitorListener;

  TestPhotobookListener photobookListener;

  PB::Photobook photobook(".", ".");
  photobook.configure(stagedImageListener.get());
  photobook.configure((PB::ImageMonitorListener *)&imageMonitorListener);

  TestDashboardListener testDashboardListener;

  photobook.configure((PB::DashboardListener *)&testDashboardListener);
  photobook.configure((PB::StagedImagesListener *)&photobookListener);
  photobook.configure((PB::PhotobookListener *)&photobookListener);

  EXPECT_CALL(testDashboardListener,
              onProjectsMetadataLoaded(std::vector<PB::ProjectMetadata>()));

  photobook.recallMetadata();

  std::vector<PB::ProjectMetadata> projectsMetadata;

  photobook.newProject("random-name");

  photobook.loadProject();
  
  EXPECT_CALL(photobookListener, post(_)).Times(AtLeast(1));

  photobook.addImportFolder("../test-data/");

  Sleep(3000);
}
