#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <pb/Config.h>
#include <pb/StagedImages.h>

class TestStagedImagesListener : public PB::StagedImagesListener {
public:
  MOCK_METHOD(void, onPicturesAdded, (int index, int size), (override));
  MOCK_METHOD(void, onPictureRemoved, (std::vector<unsigned> index),
              (override));
};

class TestStagedImagesVirtualImage : public PB::VirtualImage {
  MOCK_METHOD(PB::VirtualImageType, type, (), (const override));
};

TEST(TestStagedImages, CreateStageImages)
{
  std::shared_ptr<PB::StagedImages> stagedImages =
      std::make_shared<PB::StagedImages>();

  std::shared_ptr<TestStagedImagesListener> listener =
      std::make_shared<TestStagedImagesListener>();

  stagedImages->setListener(listener);

}


int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}