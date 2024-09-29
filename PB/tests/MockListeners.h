#pragma once

#include <regex>
#include <string>

#include <boost/bimap/bimap.hpp>
#include <boost/uuid/uuid.hpp>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <pb/PhotoBook.h>
#include <pb/Platform.h>
#include <pb/entities/LutIconInfo.h>
#include <pb/services/DatabaseService.h>
#include <pb/services/DurableHashService.h>
#include <pb/services/ProgressService.h>
#include <pb/services/ProjectManagementService.h>
#include <pb/services/ProjectSerializerService.h>

void clearProjectCache();

typedef std::vector<std::vector<PB::GenericImagePtr>>     VirtualImageMatrix;
typedef std::vector<std::shared_ptr<PB::GenericImagePtr>> VirtualImageLine;

typedef std::vector<Path> &RootsVector;

class TestPhotobookListener final : public PB::PhotobookListener {
public:
  MOCK_METHOD(void, onProjectRead, (), (override));
  MOCK_METHOD(void, onProjectRenamed, (), (override));
  MOCK_METHOD(void, onMetadataUpdated, (), (override));
  MOCK_METHOD(void, onPersistenceError, (PBDev::Error), (override));
  MOCK_METHOD(void, onExportFinished, (), (override));
  MOCK_METHOD(void, onError, (PBDev::Error), (override));

  MOCK_METHOD(void, onStagedImageRemoved, (std::vector<unsigned>), (override));

  MOCK_METHOD(void, onMappingStarted, (Path), (override));
  MOCK_METHOD(void, onMappingFinished, (Path), (override));
  MOCK_METHOD(void, onCollageThumbnailsCreated, (), (override));
  MOCK_METHOD(void, onMappingAborted, (Path), (override));

  MOCK_METHOD(void, onImageUpdated, (Path, int, int), (override));

  MOCK_METHOD(void, post, (std::function<void()>), (override));

  MOCK_METHOD(void, onCollageCreated, (unsigned, PB::GenericImagePtr),
              (override));

  MOCK_METHOD(void, onImageMapped, (PBDev::ImageToPaperId, PB::GenericImagePtr),
              (override));

  MOCK_METHOD(void, onProgressUpdate, (PB::ProgressStatus), (override));

  MOCK_METHOD(void, onLutAdded, (PB::LutIconInfo), (override));
};

class TestProjectManagementServiceListener final
    : public PB::ProjectManagementServiceListener {
public:
  ~TestProjectManagementServiceListener() = default;
  MOCK_METHOD(void, onProjectMetadataRecalled, (), (override));
  MOCK_METHOD(void, onProjectRecalled, (), (override));
};

/*
class TestPersistenceProjectListener final
    : public PB::PersistenceProjectListener {
public:
  MOCK_METHOD(void, onProjectRead,
              (std::string, std::shared_ptr<PB::Project>, VirtualImageMatrix &,
               VirtualImageLine &, RootsVector),
              (override));
  MOCK_METHOD(void, onProjectPersistenceError, (PBDev::Error), (override));
  MOCK_METHOD(void, onJsonRead, (Json), (override));
};

typedef boost::bimaps::bimap<boost::uuids::uuid, std::string> BimapWorkaround;

class TestPersistenceMetadataListener final
    : public PB::PersistenceMetadataListener {
public:
  MOCK_METHOD(void, onMetadataRead, (BimapWorkaround), (override));
  MOCK_METHOD(void, onMetadataPersistenceError, (PBDev::Error), (override));
};

class TestProjectPersistenceListener final
    : public PB::PersistenceServiceListener {
  MOCK_METHOD(void, onMetadataUpdated, (), (override));
  MOCK_METHOD(void, onProjectRead,
              (VirtualImageMatrix &, VirtualImageLine &, RootsVector),
              (override));
  MOCK_METHOD(void, onProjectRenamed, (), (override));
  MOCK_METHOD(void, onPersistenceError, (PBDev::Error), (override));
};
*/

class TestMainLoop final : public PBDev::ThreadScheduler {
public:
  ~TestMainLoop() = default;

  void post(std::function<void()> f) override
  {
    std::lock_guard<std::mutex> lock(mutex);
    q.push(f);
    ifTaskOccurs.notify_one();
  }

  void run()
  {
    std::unique_lock<std::mutex> lock(mutex);
    while (true) {
      ifTaskOccurs.wait_for(lock, std::chrono::seconds(1),
                            [this] { return !q.empty(); });
      if (q.empty()) {
        break;
      }
      auto f = q.front();
      q.pop();
      f();
    }
  }

private:
  std::queue<std::function<void()>> q;
  std::mutex                        mutex;
  std::condition_variable           ifTaskOccurs;
};

std::shared_ptr<PB::PlatformInfo> mockPlatformInfo();
std::shared_ptr<PB::DatabaseService>
mockDatabaseService(std::shared_ptr<PB::PlatformInfo> platformInfo);

std::shared_ptr<PB::ProjectSerializerService>
mockProjectSerializerService(std::shared_ptr<PB::PlatformInfo> platformInfo);

std::shared_ptr<PB::DurableHashService>
mockDurableHashService(std::shared_ptr<PB::DatabaseService> databaseService);
