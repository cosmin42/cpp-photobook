#include "MockListeners.h"

#include <pb/entities/ProjectMetadata.h>

using ::testing::_;
using ::testing::AtLeast;

TEST(TestFolderImport, Test0)
{
  clearProjectCache();

  TestPhotobookListener photobookListener;

  PB::Photobook photobook(".", ".", {1280, 720});
  photobook.configure((PB::PhotobookListener *)&photobookListener);

  EXPECT_CALL(photobookListener, onMetadataUpdated(""));
  photobook.projectManagementService()->recallMetadata();

  std::vector<PB::ProjectMetadata> projectsMetadata;

  photobook.projectManagementService()->newProject(PB::PaperSettings());

  EXPECT_CALL(photobookListener, post(_)).Times(AtLeast(1));

  photobook.importFoldersService()->addImportFolder("../test-data/");

  std::this_thread::sleep_for(std::chrono::milliseconds(3000));
}
