#include "MockListeners.h"

#include <pb/components/LinesOperationsHistory.h>

using ::testing::_;
using ::testing::AtLeast;

TEST(TestLinesOperationsHistory, Test0)
{
  PB::LinesOperationsHistory linesOperationsHistory;

  auto state = linesOperationsHistory.toString();
  EXPECT_EQ(state, "");

  linesOperationsHistory.registerAddOperation(0, 1);
  state = linesOperationsHistory.toString();
  EXPECT_EQ(state, "Add(0, 1)\n");

  linesOperationsHistory.registerAddOperation(1, 1);
  state = linesOperationsHistory.toString();
  EXPECT_EQ(state, "Add(0, 1)\nAdd(1, 1)\n");

  linesOperationsHistory.registerRemoveOperation(0, 1);
  state = linesOperationsHistory.toString();
  EXPECT_EQ(state, "Add(0, 1)\nAdd(1, 1)\nRemove(0, 1)\n");

  linesOperationsHistory.registerRemoveOperation(1, 1);
  state = linesOperationsHistory.toString();
  EXPECT_EQ(state, "Add(0, 1)\nAdd(1, 1)\nRemove(0, 1)\nRemove(1, 1)\n");

  linesOperationsHistory.registerAddOperation(0, 1);
  state = linesOperationsHistory.toString();
  EXPECT_EQ(state,
            "Add(0, 1)\nAdd(1, 1)\nRemove(0, 1)\nRemove(1, 1)\nAdd(0, 1)\n");

}