#include <gtest/gtest.h>

#include <pb/Gallery.h>

using namespace PB;

TEST(TestGallery, TestEmpty) {
  Gallery g;
  ASSERT_TRUE(!g.selectedItem().has_value());
  ASSERT_TRUE(!g.selectedImportFolder().has_value());
}
