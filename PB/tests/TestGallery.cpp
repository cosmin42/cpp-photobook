#include <gtest/gtest.h>

#include <pb/Gallery.h>

using namespace PB;

TEST(TestGallery, TestEmpty) {
  std::vector<Path> testPaths = {"a", "b", "c", "d"};

  Gallery g(testPaths);
  ASSERT_TRUE(!g.selectedItem().has_value());
  ASSERT_TRUE(!g.selectedImportFolder().has_value());
}