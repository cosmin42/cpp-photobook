#include <gtest/gtest.h>

#include <pb/Gallery.h>
#include <pb/RegularImage.h>
#include <pb/TextImage.h>

using namespace PB;

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
    if (std::filesystem::is_regular_file(p)) {
      auto newRegularImage = std::make_shared<PB::RegularImage>(Thumbnails(p));
      newRegularImage->setFullSizePath(p);
      imagesSet.push_back(newRegularImage);
    }
    else if (std::filesystem::is_directory(p)) {
      auto textImage = std::make_shared<PB::TextImage>(p.stem().string());
      textImage->setFullSizePath(p);
      imagesSet.push_back(textImage);
    }
    else {
      PB::basicAssert(false);
    }
  }

  imageSupport.addImage("a", mediaMap, imagesSet);

  g.selectImportFolder(0, imageSupport.unstagedIterator(0));

  auto maybeSelected = g.selectedItem();

  ASSERT_TRUE(maybeSelected != nullptr);

  ASSERT_TRUE(maybeSelected->fullSizePath().string() == "a/b");
  g.setPhotoLinePosition(5);
  maybeSelected = g.selectedItem();
  ASSERT_TRUE(maybeSelected->fullSizePath().string() == "b/c");
}
