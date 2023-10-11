#include <gtest/gtest.h>

#include <pb/DataManager.h>

using namespace PB;

class MockImageSupportListener final : public ImageSupportListener {
public:
  void importFolderAdded(int index) override { mIndex = index; }
  void stagePhotosUpdated() override {}

  const int index() { return mIndex; }

private:
  int mIndex = -1;
};

TEST(TestDataManager, TestEmpty)
{
  std::shared_ptr<MockImageSupportListener> listener =
      std::make_shared<MockImageSupportListener>();
  PB::ImageSupport imageSupport;
  imageSupport.setListener(listener);

  ASSERT_TRUE(listener->index() == -1);

  imageSupport.addFullPaths("a", std::vector<Path>{"a/b", "a/c", "b/c"});

  ASSERT_TRUE(listener->index() == 0);

  auto iterator = imageSupport.stagedIterator();
  ASSERT_TRUE(iterator.size() == 0);

  auto unstagedIterator = imageSupport.unstagedIterator(0);
  ASSERT_TRUE(unstagedIterator.size() == 3);

  unstagedIterator = imageSupport.unstagedIterator(Path("a"));
  ASSERT_TRUE(unstagedIterator.size() == 3);
}

TEST(TestDataManager, TestAddingToSTaging)
{
  std::shared_ptr<MockImageSupportListener> listener =
      std::make_shared<MockImageSupportListener>();
  PB::ImageSupport imageSupport;
  imageSupport.setListener(listener);

  ASSERT_TRUE(listener->index() == -1);

  imageSupport.addFullPaths("a", std::vector<Path>{"a/b", "a/c", "b/c"});

  ASSERT_TRUE(listener->index() == 0);

  auto iterator = imageSupport.stagedIterator();
  ASSERT_TRUE(iterator.size() == 0);

  auto unstagedIterator = imageSupport.unstagedIterator(0);
  ASSERT_TRUE(unstagedIterator.size() == 3);

  unstagedIterator = imageSupport.unstagedIterator(Path("a"));
  ASSERT_TRUE(unstagedIterator.size() == 3);

  Thumbnails toBeStaged("a/b", "b", "c");

  imageSupport.stagePhoto(toBeStaged);

  auto stagedIterator = imageSupport.stagedIterator();

  ASSERT_TRUE(stagedIterator.size() == 1);

  imageSupport.unstagePhoto(0);
  stagedIterator = imageSupport.stagedIterator();

  ASSERT_TRUE(stagedIterator.size() == 0);
}