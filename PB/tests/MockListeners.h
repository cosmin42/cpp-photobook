#pragma once

#include <regex>
#include <string>

#include <boost/bimap/bimap.hpp>
#include <boost/uuid/uuid.hpp>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <pb/DatabaseService.h>
#include <pb/DurableHashService.h>
#include <pb/LutIconInfo.h>
#include <pb/PhotoBook.h>
#include <pb/Platform.h>
#include <pb/ProgressManager.h>
#include <pb/ProjectManagementSystem.h>

void clearProjectCache();

typedef std::vector<std::vector<std::shared_ptr<PB::VirtualImage>>>
                                                       VirtualImageMatrix;
typedef std::vector<std::shared_ptr<PB::VirtualImage>> VirtualImageLine;

typedef std::vector<Path> &RootsVector;

class TestPhotobookStagedImagesListener : public PB::StagedImagesListener {
public:
  MOCK_METHOD(void, onPicturesAdded, (int index, int size), (override));
  MOCK_METHOD(void, onPictureRemoved, (std::vector<unsigned> index),
              (override));
};

class MockPhotobookImageMonitorListener final
    : public PB::ImageMonitorListener {
public:
  MOCK_METHOD(void, onImportFolderAdded, (), (override));
  MOCK_METHOD(void, onImportFolderRemoved, (unsigned index), (override));
  MOCK_METHOD(void, onRefresh, (), (override));
  MOCK_METHOD(void, onCleared, (), (override));
};

class TestPhotobookListener final : public PB::PhotobookListener {
public:
  MOCK_METHOD(void, onProjectRead, (), (override));
  MOCK_METHOD(void, onProjectRenamed, (), (override));
  MOCK_METHOD(void, onMetadataUpdated, (), (override));
  MOCK_METHOD(void, onPersistenceError, (PBDev::Error), (override));
  MOCK_METHOD(void, onExportFinished, (), (override));
  MOCK_METHOD(void, onError, (PBDev::Error), (override));

  MOCK_METHOD(void, onStagedImageAdded,
              (std::vector<std::shared_ptr<PB::VirtualImage>> photos,
               int                                            index),
              (override));
  MOCK_METHOD(void, onStagedImageRemoved, (std::vector<unsigned>), (override));

  MOCK_METHOD(void, onMappingStarted, (Path), (override));
  MOCK_METHOD(void, onMappingFinished, (Path), (override));
  MOCK_METHOD(void, onCollageThumbnailsCreated, (), (override));
  MOCK_METHOD(void, onMappingAborted, (Path), (override));

  MOCK_METHOD(void, onImageUpdated, (Path, int, int), (override));

  MOCK_METHOD(void, post, (std::function<void()>), (override));
  MOCK_METHOD(void, onCollageCreated,
              (unsigned, std::shared_ptr<PB::VirtualImage>), (override));

  MOCK_METHOD(void, onImageMapped,
              (PBDev::ImageToPaperId, std::shared_ptr<PB::VirtualImage>),
              (override));

  MOCK_METHOD(void, onProgressUpdate, (PB::ProgressStatus), (override));

  MOCK_METHOD(void, onLutAdded, (PB::LutIconInfo), (override));
};

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

class TestProjectManagementSystemListener final
    : public PB::ProjectManagementSystemListener {
public:
  ~TestProjectManagementSystemListener() = default;
  MOCK_METHOD(void, onProjectMetadataRecalled, (), (override));
};

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
std::shared_ptr<PB::DurableHashService>
mockDurableHashService(std::shared_ptr<PB::DatabaseService> databaseService);