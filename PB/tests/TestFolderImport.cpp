#include "MockListeners.h"


TEST(TestFolderImport, Test0)
{
  clearProjectCache();

  std::shared_ptr<PB::StagedImagesListener> stagedImageListener =
      std::make_shared<TestPhotobookStagedImagesListener>();

  MockPhotobookImageMonitorListener imageMonitorListener;

  std::shared_ptr<PB::PhotobookListener> photobookListener =
      std::make_shared<TestPhotobookListener>();

  PB::Photobook photobook(".");
  photobook.configure(stagedImageListener.get());
  photobook.configure((PB::ImageMonitorListener*)&imageMonitorListener);

  TestDashboardListener testDashboardListener;

  photobook.configure((PB::DashboardListener *)&testDashboardListener);
  photobook.configure(photobookListener);

  EXPECT_CALL(testDashboardListener,
              onProjectsMetadataLoaded(std::vector<PB::ProjectMetadata>()));

  photobook.recallMetadata();

  std::vector<PB::ProjectMetadata> projectsMetadata;

  photobook.newProject();

  photobook.loadProject();

  EXPECT_CALL(imageMonitorListener, onImportFolderAdded());

  photobook.addImportFolder("../test-data/");

  Sleep(5000);


}
