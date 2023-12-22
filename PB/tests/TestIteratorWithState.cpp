#include <gtest/gtest.h>

#include <pb/util/IteratorWithState.h>

using namespace PB;

TEST(TestIteratorWithState, TestEmpty)
{
  auto it = IteratorWithState<std::vector<int>>();
  ASSERT_TRUE(it.size() == 0);
  ASSERT_TRUE(!it.valid());
}

TEST(TestIteratorWithState, TestFilled)
{
  std::vector<int> v = {1, 3, 5, 7, 9};
  auto             it = IteratorWithState<std::vector<int>>(v);
  ASSERT_TRUE(it.size() == 5);
  ASSERT_TRUE(it.valid());
}

TEST(TestIteratorWithState, TestDereference)
{
  std::vector<int> v = {1, 3, 5, 7, 9};
  auto             it = IteratorWithState<std::vector<int>>(v);
  ASSERT_TRUE(it.size() == 5);
  ASSERT_TRUE(it.valid());
  ASSERT_TRUE(it.current() == 1);
}

TEST(TestIteratorWithState, TestIncrementDecrmeent)
{
  std::vector<int> v = {1, 3, 5, 7, 9};
  auto             it = IteratorWithState<std::vector<int>>(v);
  ASSERT_TRUE(it.size() == 5);
  ASSERT_TRUE(it.valid());
  ASSERT_TRUE(it.current() == 1);
}

TEST(TestIteratorWithState, TestIndexOperator)
{
  std::vector<int> v = {1, 3, 5, 7, 9};
  auto             it = IteratorWithState<std::vector<int>>(v);
  ASSERT_TRUE(it.size() == 5);
  ASSERT_TRUE(it.valid());
  ASSERT_TRUE(it.current() == 1);
  it = it[4];
  ASSERT_TRUE(it.current() == 9);
  it = it[15];
  ASSERT_TRUE(it.current() == 1);
}
