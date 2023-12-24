#include <gmock/gmock.h>
#include <gtest/gtest.h>

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

TEST(TestPhotobook, TestProject)
{
  std::shared_ptr<PB::StagedImagesListener> stagedImageListener =
      std::make_shared<TestPhotobookStagedImagesListener>();

  std::shared_ptr<PB::ImageMonitorListener> imageMonitorListener =
      std::make_shared<MockPhotobookImageMonitorListener>();

  PB::Photobook photobook(".");
  photobook.configure(stagedImageListener.get());
  photobook.configure(imageMonitorListener.get());
}