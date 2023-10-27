#include <gtest/gtest.h>

#include <pb/Config.h>
#include <pb/Project.h>
#include <pb/persistence/Persistence.h>

using namespace PB;

TEST(TestProjectPersistence, CreateNewProject)
{
  std::filesystem::remove("database.db");

  PB::SQLitePersistence centralPersistence(".");

  centralPersistence.connect();

  ASSERT_TRUE(std::filesystem::exists("database.db"));

  PB::Project project(Path("."));

  auto uuid = project.details().uuid();

  auto supportDirName = project.details().supportDirName();

  auto parentDirectory = project.details().parentDirectory();

  ASSERT_TRUE(supportDirName ==
              boost::uuids::to_string(project.details().uuid()));

  PB::FilePersistence filePersistence(project.details().projectFile());
  auto                jsonOrError =
      PB::Text::serialize<ProjectDetails>(0, {"root", project.details()});
  assert(!std::holds_alternative<Error>(jsonOrError));

  ASSERT_TRUE(std::get<Json>(jsonOrError).at("root").at("project-uuid") ==
              supportDirName);
  ASSERT_TRUE(std::get<Json>(jsonOrError).at("root").at("project-name") ==
              supportDirName);
  ASSERT_TRUE(std::get<Json>(jsonOrError).at("root").at("project-path") ==
              parentDirectory.string());

  filePersistence.write(
      std::get<Json>(jsonOrError).at("root"),
      [supportDirName, parentDirectory](std::optional<Error> maybeError) {
        ASSERT_TRUE(!maybeError.has_value());
      });
  std::pair<std::string, std::string> entry = {
      boost::uuids::to_string(project.details().uuid()),
      project.details().supportFolder().string()};

  centralPersistence.write(entry, [](std::optional<Error> maybeError) {
    ASSERT_TRUE(!maybeError.has_value());
  });

  centralPersistence.read(
      [entry{entry}](
          std::variant<std::unordered_map<std::string, std::string>, Error>
              mapOrError) {
        ASSERT_TRUE(!std::holds_alternative<Error>(mapOrError));

        auto &map =
            std::get<std::unordered_map<std::string, std::string>>(mapOrError);

        ASSERT_TRUE(map.size() == 1);

        auto &[key, value] = entry;

        auto &[readKey, readValue] = *map.begin();

        ASSERT_TRUE(key == readKey);
        ASSERT_TRUE(value == readValue);
      });
}

TEST(TestProjectPersistence, CheckProjectPersistence)
{
  PB::Project project(Path("."));

  auto uuid = project.details().uuid();

  auto supportDirName = project.details().supportDirName();

  auto parentDirectory = project.details().parentDirectory();

  ASSERT_TRUE(supportDirName ==
              boost::uuids::to_string(project.details().uuid()));

  PB::FilePersistence filePersistence(project.details().projectFile());

  auto jsonOrError =
      PB::Text::serialize<ProjectDetails>(0, {"root", project.details()});
  assert(!std::holds_alternative<Error>(jsonOrError));

  ASSERT_TRUE(std::get<Json>(jsonOrError).at("root").at("project-uuid") ==
              supportDirName);
  ASSERT_TRUE(std::get<Json>(jsonOrError).at("root").at("project-name") ==
              supportDirName);
  ASSERT_TRUE(std::get<Json>(jsonOrError).at("root").at("project-path") ==
              parentDirectory.string());

  filePersistence.write(std::get<Json>(jsonOrError).at("root"),
                        [](std::optional<Error> maybeError) {
                          ASSERT_TRUE(!maybeError.has_value());
                        });
  filePersistence.read(
      [supportDirName, parentDirectory](std::variant<Json, Error> mapOrError) {
        ASSERT_TRUE(!std::holds_alternative<Error>(mapOrError));
        auto &map = std::get<Json>(mapOrError);
        auto projectDetailsOrError = PB::Text::deserialize<ProjectDetails>(map);

        ASSERT_TRUE(!std::holds_alternative<Error>(projectDetailsOrError));
        auto &projectDetails = std::get<ProjectDetails>(projectDetailsOrError);
        ASSERT_TRUE(projectDetails.supportDirName() == supportDirName);
        ASSERT_TRUE(projectDetails.parentDirectory() == parentDirectory);
      });
}