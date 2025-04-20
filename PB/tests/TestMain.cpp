#include <gtest/gtest.h>

#include <pb/Config.h>

int main(int argc, char **argv)
{
  PB::Noir::inst();
  ::testing::GTEST_FLAG(filter) = "TestFolderImport.*";
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}