#include "MockListeners.h"

#include <pb/components/LinesOperationsHistory.h>

using ::testing::_;
using ::testing::AtLeast;

TEST(TestLinesOperationsHistory, Test0)
{
  PB::LinesOperationsHistory linesOperationsHistory;

  auto state = linesOperationsHistory.toString();
  EXPECT_EQ(state, "");
  unsigned              version = 0;
  std::vector<unsigned> values = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

  // {10, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  linesOperationsHistory.registerAddOperation(0, 1);
  values.insert(values.begin() + 0, 10);
  version++;

  auto updatedIndex = linesOperationsHistory.updateIndex(0, version, 0);
  EXPECT_EQ(updatedIndex, 1);

  // {10, 0, 1, 2, 11, 12, 13, 3, 4, 5, 6, 7, 8, 9};
  linesOperationsHistory.registerAddOperation(4, 3);
  values.insert(values.begin() + 4, 11);
  values.insert(values.begin() + 5, 12);
  values.insert(values.begin() + 6, 13);
  version++;

  updatedIndex = linesOperationsHistory.updateIndex(0, version, 0);
  EXPECT_EQ(updatedIndex, 1);

  updatedIndex = linesOperationsHistory.updateIndex(0, version, 4);
  EXPECT_EQ(updatedIndex, 8);

  updatedIndex = linesOperationsHistory.updateIndex(0, version, 5);
  EXPECT_EQ(updatedIndex, 9);

  // {10, 0, 1, 2, 13, 3, 4, 5, 6, 7, 8, 9};
  linesOperationsHistory.registerRemoveOperation(4, 2);
  values.erase(values.begin() + 4, values.begin() + 6);
  version++;

  updatedIndex = linesOperationsHistory.updateIndex(0, version, 0);
  EXPECT_EQ(updatedIndex, 1);

  updatedIndex = linesOperationsHistory.updateIndex(0, version, 4);
  EXPECT_EQ(updatedIndex, 6);

  updatedIndex = linesOperationsHistory.updateIndex(0, version, 5);
  EXPECT_EQ(updatedIndex, 7);

  // {10, 0, 1, 2, 4, 5, 6, 7, 8, 9};
  linesOperationsHistory.registerRemoveOperation(4, 2);
  values.erase(values.begin() + 4, values.begin() + 6);
  version++;

  updatedIndex = linesOperationsHistory.updateIndex(0, version, 0);
  EXPECT_EQ(updatedIndex, 1);

  updatedIndex = linesOperationsHistory.updateIndex(0, version, 4);
  EXPECT_EQ(updatedIndex, std::nullopt);

  updatedIndex = linesOperationsHistory.updateIndex(0, version, 5);
  EXPECT_EQ(updatedIndex, 5);
}