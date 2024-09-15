#include <gtest/gtest.h>

#include <algorithm>

#include <pb/FileComparator.h>

using namespace PB;

TEST(TestFileComparator, ExtractProfile)
{
  std::vector<std::string> testV = {"",       "FDS",          ".svfvdfbvd", ".",
                                    "546456", "53534bdfbgbf", ".2023.fdfsd"};
  for (auto &str : testV) {
    auto opt = PB::CustomComparator::extractPrefix(str);

    ASSERT_TRUE(!opt.has_value());
  }

  std::vector<std::string> testV2 = {"2023.", "12.20.2023.vfdvdfvfd"};
  std::vector<std::string> references = {"2023", "12.20.2023"};
  auto                     count = 0;
  for (auto &str : testV2) {
    auto opt = PB::CustomComparator::extractPrefix(str);

    ASSERT_TRUE(opt.has_value());

    ASSERT_EQ(opt.value(), references.at(count));

    count++;
  }
}

TEST(TestFileComparator, TokenizeDate)
{
  std::vector<std::string> testV2 = {"2023.", "12.20.2023.vfdvdfvfd"};
  std::vector<std::vector<std::string>> references = {{"2023"},
                                                      {"2023", "20", "12"}};

  for (auto i = 0; i < testV2.size(); ++i) {
    auto opt = PB::CustomComparator::extractPrefix(testV2.at(i));

    ASSERT_TRUE(opt.has_value());

    auto q = PB::CustomComparator::tokenizeDate(opt.value());

    ASSERT_TRUE(q.size() == references.at(i).size());

    auto j = 0;
    while (!q.empty()) {
      auto str = q.top();
      q.pop();
      ASSERT_EQ(str, references.at(i).at(j));
      j++;
    }
  }
}

TEST(TestFileComparator, EmptyString)
{
  std::vector<std::string> result = {""};
  std::sort(result.begin(), result.end(), PB::CustomComparator());

  const std::vector reference = {""};

  ASSERT_TRUE(std::equal(result.begin(), result.end(), reference.cbegin(),
                         reference.cend()));
}

TEST(TestFileComparator, Invalid)
{
  // std::vector<std::string> result = {"", "324bjk-vsdfvdf3-534534"};
  std::vector<std::string> result = {"",  "ABCD",    "X",
                                     ")", "DSC.JPG", "324bjk-vsdfvdf3-534534"};
  std::sort(result.begin(), result.end(), PB::CustomComparator());

  // const std::vector reference = {"", "324bjk-vsdfvdf3-534534"};
  const std::vector reference = {"",     ")",       "324bjk-vsdfvdf3-534534",
                                 "ABCD", "DSC.JPG", "X"};

  ASSERT_TRUE(std::equal(result.begin(), result.end(), reference.cbegin(),
                         reference.cend()));
}

TEST(TestFileComparator, Valid)
{
  // std::vector<std::string> result = {"", "324bjk-vsdfvdf3-534534"};
  std::vector<std::string> result = {"2022.DSC.JPG", "12.2023.DSC.JPG",
                                     "01.12.2023.DSC.JPG",
                                     "02.12.1999.DSC.JPG"};
  std::sort(result.begin(), result.end(), PB::CustomComparator());

  // const std::vector reference = {"", "324bjk-vsdfvdf3-534534"};
  const std::vector reference = {"02.12.1999.DSC.JPG", "2022.DSC.JPG",
                                 "12.2023.DSC.JPG", "01.12.2023.DSC.JPG"};

  ASSERT_TRUE(std::equal(result.begin(), result.end(), reference.cbegin(),
                         reference.cend()));
}