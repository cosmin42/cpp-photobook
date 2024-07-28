#include "MockListeners.h"

#include <pb/project/ProjectMetadata.h>

using ::testing::_;
using ::testing::AtLeast;

TEST(TestFolderImport, Test0)
{
  clearProjectCache();

  std::shared_ptr<PB::StagedImagesListener> stagedImageListener =
      std::make_shared<TestPhotobookStagedImagesListener>();

  MockPhotobookImageMonitorListener imageMonitorListener;

  TestPhotobookListener photobookListener;

  PB::Photobook photobook(".", ".", {1280, 720});
  photobook.configure(stagedImageListener.get());
  photobook.configure((PB::ImageMonitorListener *)&imageMonitorListener);
  photobook.configure((PB::StagedImagesListener *)&photobookListener);
  photobook.configure((PB::PhotobookListener *)&photobookListener);

  EXPECT_CALL(photobookListener, onMetadataUpdated());
  photobook.recallMetadata();

  std::vector<PB::ProjectMetadata> projectsMetadata;

  photobook.newProject("random-name", PB::PaperSettings());

  EXPECT_CALL(photobookListener, post(_)).Times(AtLeast(1));

  photobook.addImportFolder("../test-data/");

  std::this_thread::sleep_for(std::chrono::milliseconds(3000));
}
