#include <gtest/gtest.h>

#include <pb/Gallery.h>
#include <pb/RegularImage.h>
#include <pb/TextImage.h>

using namespace PB;
/*

TEST(TestGallery, TestEmpty)
{
  Gallery g;
  ASSERT_TRUE(g.selectedItem() == nullptr);
  g.setPhotoLinePosition(0);
  ASSERT_TRUE(g.selectedItem() == nullptr);
  g.setPhotoLinePosition(10);
  ASSERT_TRUE(g.selectedItem() == nullptr);
}

TEST(TestGallery, Test2)
{
  Gallery g;
  ASSERT_TRUE(g.selectedItem() == nullptr);

  PB::ImageSupport imageSupport;
  imageSupport.setListener(g.slot());

  std::vector<Path> mediaMap = std::vector<Path>{"a/b", "a/c", "b/c"};

  std::vector<std::shared_ptr<VirtualImage>> imagesSet;
  for (auto p : mediaMap) {
    auto newRegularImage = std::make_shared<PB::RegularImage>(Thumbnails(p));
    newRegularImage->setFullSizePath(p);
    imagesSet.push_back(newRegularImage);
  }

  imageSupport.addImage("a", mediaMap, imagesSet);

  g.selectImportFolder(0, imageSupport.unstagedIterator(0));

  auto maybeSelected = g.selectedItem();

  ASSERT_TRUE(maybeSelected != nullptr);

  ASSERT_TRUE(maybeSelected->resources().full.string() == "a/b");
  g.setPhotoLinePosition(5);
  maybeSelected = g.selectedItem();
  ASSERT_TRUE(maybeSelected->resources().full.string() == "b/c");
}
*/