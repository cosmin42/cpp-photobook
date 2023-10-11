#include <gtest/gtest.h>

#include <pb/Gallery.h>

using namespace PB;

TEST(TestGallery, TestEmpty) {
  Gallery g;
  ASSERT_TRUE(!g.selectedItem().has_value());
  g.setPosition(0);
  ASSERT_TRUE(!g.selectedItem().has_value());
  g.navigateLeft();
  ASSERT_TRUE(!g.selectedItem().has_value());
  g.navigateRight();
  ASSERT_TRUE(!g.selectedItem().has_value());
  g.setPosition(10);
  ASSERT_TRUE(!g.selectedItem().has_value());
}

TEST(TestGallery, Test2) {
  Gallery g;
  ASSERT_TRUE(!g.selectedItem().has_value());

  PB::ImageSupport imageSupport;
  imageSupport.setListener(g.slot());

  imageSupport.addFullPaths("a", std::vector<Path>{"a/b", "a/c", "b/c"});

  g.selectImportFolder(0, imageSupport.unstagedIterator(0));

  auto maybeSelected = g.selectedItem();

  ASSERT_TRUE(maybeSelected.has_value());

  ASSERT_TRUE(maybeSelected->fullPath.string() == "a/b");
  g.navigateRight();
  maybeSelected = g.selectedItem();
  ASSERT_TRUE(maybeSelected->fullPath.string() == "a/c");
  g.setPosition(5);
  maybeSelected = g.selectedItem();
  ASSERT_TRUE(maybeSelected->fullPath.string() == "b/c");
}
