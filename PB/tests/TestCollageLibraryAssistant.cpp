#include <gtest/gtest.h>

#include <pb/CollageLibraryAssistant.h>

using namespace PB;

TEST(TestCollageAssistant, ExtractProfile)
{
  CollageLibraryAssistant assistant(".", ".");

  assistant.createNumberedImages({2480, 1754});
}