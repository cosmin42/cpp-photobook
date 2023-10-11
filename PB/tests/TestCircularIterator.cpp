#include <gtest/gtest.h>

#include <pb/util/CircularIterator.h>

using namespace PB;

TEST(TestFileComparator, TestEmpty)
{
  auto it = CircularIterator<std::vector<int>>();
  ASSERT_TRUE(it.size() == 0);
  ASSERT_TRUE(!it.valid());
}

TEST(TestFileComparator, TestFilled)
{
  std::vector<int> v = {1, 3, 5, 7, 9};
  auto             it = CircularIterator<std::vector<int>>(v);
  ASSERT_TRUE(it.size() == 5);
  ASSERT_TRUE(it.valid());
}

TEST(TestFileComparator, TestDereference)
{
  std::vector<int> v = {1, 3, 5, 7, 9};
  auto             it = CircularIterator<std::vector<int>>(v);
  ASSERT_TRUE(it.size() == 5);
  ASSERT_TRUE(it.valid());
  ASSERT_TRUE(it.current() == 1);
}

TEST(TestFileComparator, TestIncrementDecrmeent)
{
  std::vector<int> v = {1, 3, 5, 7, 9};
  auto             it = CircularIterator<std::vector<int>>(v);
  ASSERT_TRUE(it.size() == 5);
  ASSERT_TRUE(it.valid());
  ASSERT_TRUE(it.current() == 1);
  it = it.next();
  ASSERT_TRUE(it.current() == 3);
  it = it.previous();
  ASSERT_TRUE(it.current() == 1);
  it = it.previous();
  ASSERT_TRUE(it.current() == 9);
}

TEST(TestFileComparator, TestIndexOperator)
{
  std::vector<int> v = {1, 3, 5, 7, 9};
  auto             it = CircularIterator<std::vector<int>>(v);
  ASSERT_TRUE(it.size() == 5);
  ASSERT_TRUE(it.valid());
  ASSERT_TRUE(it.current() == 1);
  it = it[4];
  ASSERT_TRUE(it.current() == 9);
  it = it[15];
  ASSERT_TRUE(it.current() == 1);
}
