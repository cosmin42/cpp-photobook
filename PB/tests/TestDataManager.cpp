#include <gtest/gtest.h>

#include <pb/DataManager.h>
#include <pb/RegularImage.h>
#include <pb/TextImage.h>

using namespace PB;

class MockImageSupportListener final : public ImageSupportListener {
public:
  void importFolderAdded(
      Path path,
      CircularIterator<std::vector<std::shared_ptr<VirtualImage>>>) override
  {
    mImportFolders.push_back(path);
  }
  void stagePhotosUpdated(
      CircularIterator<std::vector<std::shared_ptr<VirtualImage>>>) override
  {
  }

  const int size() { return (int)mImportFolders.size(); }

private:
  std::vector<Path> mImportFolders;
};

TEST(TestDataManager, TestEmpty)
{
  std::shared_ptr<MockImageSupportListener> listener =
      std::make_shared<MockImageSupportListener>();
  PB::ImageSupport imageSupport;
  imageSupport.setListener(listener);

  ASSERT_TRUE(listener->size() == 0);

  std::vector<Path> mediaMap = std::vector<Path>{"a/b", "a/c", "b/c"};

  std::vector<std::shared_ptr<VirtualImage>> imagesSet;
  for (auto p : mediaMap) {
    auto newRegularImage = std::make_shared<PB::RegularImage>(Thumbnails(p));
    newRegularImage->setFullSizePath(p);
    imagesSet.push_back(newRegularImage);
  }

  imageSupport.addImage("a", std::vector<Path>{"a/b", "a/c", "b/c"}, imagesSet);

  ASSERT_TRUE(listener->size() == 1);

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

  ASSERT_TRUE(listener->size() == 0);

  std::vector<Path> mediaMap = std::vector<Path>{"a/b", "a/c", "b/c"};

  std::vector<std::shared_ptr<VirtualImage>> imagesSet;
  for (auto p : mediaMap) {
    auto newRegularImage = std::make_shared<PB::RegularImage>(Thumbnails(p));
    newRegularImage->setFullSizePath(p);
    imagesSet.push_back(newRegularImage);
  }

  imageSupport.addImage("a", std::vector<Path>{"a/b", "a/c", "b/c"}, imagesSet);

  ASSERT_TRUE(listener->size() == 1);

  auto iterator = imageSupport.stagedIterator();
  ASSERT_TRUE(iterator.size() == 0);

  auto unstagedIterator = imageSupport.unstagedIterator(0);
  ASSERT_TRUE(unstagedIterator.size() == 3);

  unstagedIterator = imageSupport.unstagedIterator(Path("a"));
  ASSERT_TRUE(unstagedIterator.size() == 3);

  Thumbnails toBeStaged("a/b", "b", "c");

  auto regularImage = std::make_shared<PB::RegularImage>(toBeStaged);

  imageSupport.stagePhoto({regularImage});

  auto stagedIterator = imageSupport.stagedIterator();

  ASSERT_TRUE(stagedIterator.size() == 1);

  imageSupport.unstagePhoto({0});
  stagedIterator = imageSupport.stagedIterator();

  ASSERT_TRUE(stagedIterator.size() == 0);
}