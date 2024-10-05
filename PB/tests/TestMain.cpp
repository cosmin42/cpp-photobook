#include <gtest/gtest.h>

#include <pb/Config.h>

int main(int argc, char **argv)
{
  try {
    PB::OneConfig::LOGGER = std::make_shared<spdlog::logger>(
        "console_logger",
        std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
    PB::OneConfig::LOGGER->set_level(spdlog::level::debug);
    PB::OneConfig::LOGGER->info("Log init succeeded");
  }
  catch (const spdlog::spdlog_ex &ex) {
    std::cout << "Log init failed: " << ex.what() << std::endl;
  }
  ::testing::GTEST_FLAG(filter) = "TestSerializer.*";
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}