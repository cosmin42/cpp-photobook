#pragma once

#include <functional>
#include <regex>
#include <string>

#include <boost/bimap/bimap.hpp>
#include <boost/uuid/uuid.hpp>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <pb/PhotoBook.h>
#include <pb/Platform.h>
#include <pb/entities/LutIconInfo.h>
#include <pb/export/ExportService.h>
#include <pb/infra/TSQueue.h>
#include <pb/services/DatabaseService.h>
#include <pb/services/DurableHashService.h>
#include <pb/services/ProgressService.h>
#include <pb/services/ProjectManagementService.h>
#include <pb/services/ProjectSerializerService.h>

void clearProjectCache();

typedef std::vector<std::vector<PB::GenericImagePtr>>     VirtualImageMatrix;
typedef std::vector<std::shared_ptr<PB::GenericImagePtr>> VirtualImageLine;

typedef std::vector<Path> &RootsVector;

using namespace PB::Service;

class TestPhotobookListener final : public PB::PhotobookListener {
public:
  MOCK_METHOD(void, onProjectRead, (), (override));
  MOCK_METHOD(void, onMetadataUpdated, (std::string), (override));
  MOCK_METHOD(void, onPersistenceError, (PBDev::Error), (override));
  MOCK_METHOD(void, onExportFinished, (), (override));
  MOCK_METHOD(void, onError, (PBDev::Error), (override));

  MOCK_METHOD(void, onStagedImageRemoved, (std::vector<unsigned>), (override));

  MOCK_METHOD(void, onMappingStarted, (Path), (override));
  MOCK_METHOD(void, onMappingFinished, (Path, unsigned), (override));
  MOCK_METHOD(void, onCollageThumbnailsCreated, (), (override));
  MOCK_METHOD(void, onMappingAborted, (Path), (override));

  MOCK_METHOD(void, onImageUpdated, (Path, int, int), (override));

  MOCK_METHOD(void, post, (std::function<void()>), (override));

  MOCK_METHOD(void, onCollageCreated, (PB::GenericImagePtr, PBDev::Path),
              (override));

  MOCK_METHOD(void, onImageMapped,
              (PBDev::ImageToPaperId, PB::GenericImagePtr, PBDev::Path),
              (override));

  MOCK_METHOD(void, onProgressUpdate, (PB::ProgressStatus), (override));

  MOCK_METHOD(void, onLutAdded, (PB::LutIconInfo), (override));
  MOCK_METHOD(void, onLutApplied,
              (PBDev::LutApplicationId, PB::GenericImagePtr, PBDev::Path),
              (override));
  MOCK_METHOD(void, onLutAppliedInMemory,
              (PBDev::LutApplicationId, std::shared_ptr<cv::Mat>), (override));

  MOCK_METHOD(void, onLutAppliedOnDiskInplace, (PBDev::LutApplicationId),
              (override));

  MOCK_METHOD(void, onLutAppliedOnDisk,
              (PBDev::LutApplicationId, PB::GenericImagePtr, PBDev::Path),
              (override));

  MOCK_METHOD(void, onEffectsApplied,
              (PBDev::EffectId, PB::GenericImagePtr, PBDev::Path), (override));

  MOCK_METHOD(void, onImageCopied,
              (PBDev::ImageToPaperId, PB::GenericImagePtr, PBDev::Path),
              (override));

  MOCK_METHOD(void, onEffectsAppliedInplace, (PBDev::EffectId), (override));
};

class TestProjectManagementServiceListener final
    : public ProjectManagementServiceListener {
public:
  ~TestProjectManagementServiceListener() = default;
  MOCK_METHOD(void, onProjectMetadataRecalled, (std::string), (override));
  MOCK_METHOD(void, onProjectRecalled, (), (override));
};

class TestLutServiceListener final : public LutServiceListener {
public:
  MOCK_METHOD(void, onLutAdded, (PB::LutIconInfo), (override));
  MOCK_METHOD(void, onLutApplied,
              (PBDev::LutApplicationId, PB::GenericImagePtr), (override));
  MOCK_METHOD(void, onLutAppliedInMemory,
              (PBDev::LutApplicationId, std::shared_ptr<cv::Mat>), (override));
  MOCK_METHOD(void, onLutAppliedOnDiskInplace, (PBDev::LutApplicationId),
              (override));

  MOCK_METHOD(void, onLutAppliedOnDisk,
              (PBDev::LutApplicationId, PB::GenericImagePtr), (override));
};

class TestCollageMakerListener final : public CollageMakerListener {
public:
  MOCK_METHOD(void, onCollageCreated, (PB::GenericImagePtr), (override));
  MOCK_METHOD(void, onCollageMakerError, (), (override));
};

class TestCollageThumbnailsMakerListener final
    : public CollageThumbnailsMakerListener {
public:
  MOCK_METHOD(void, onThumbnailsCreated, (), (override));
  MOCK_METHOD(void, onCollageThumbnailsMakerError, (), (override));
};

class TestExportServiceListener final : public PB::ExportListener {
public:
  MOCK_METHOD(void, onExportComplete, (Path), (override));
  MOCK_METHOD(void, onExportAborted, (Path), (override));
  MOCK_METHOD(void, onExportUpdate, (Path), (override));
};

typedef std::unordered_map<PBDev::ImageId, PB::GenericImagePtr,
                           boost::hash<PBDev::ImageId>>
    ImageMap;

class TestImportFoldersServiceListener final
    : public ImportFoldersServiceListener {
public:
  MOCK_METHOD(void, onMappingFinished, (Path, std::vector<Path>), (override));
  MOCK_METHOD(void, onSearchingFinished, (Path, ImageMap), (override));

  void onImageProcessed(PBDev::ImageId imageId, Path root,
                        PB::GenericImagePtr image) override
  {
    Path base = Path("..") / Path("..") / "PB" / "test-data" /
                "test-local-state" / "projects" /
                "93a24d3d-edce-48fd-a361-b63ce675039d" / "thumbnail-images";
    std::filesystem::remove(base / image->full());
    std::filesystem::remove(base / image->medium());
    std::filesystem::remove(base / image->smaLL());
  }

  MOCK_METHOD(void, onImportError, (PBDev::Error), (override));
};

/*
class TestPersistenceProjectListener final
    : public PB::PersistenceProjectListener {
public:
  MOCK_METHOD(void, onProjectRead,
              (std::string, std::shared_ptr<PB::Project>, VirtualImageMatrix
&, VirtualImageLine &, RootsVector), (override)); MOCK_METHOD(void,
onProjectPersistenceError, (PBDev::Error), (override)); MOCK_METHOD(void,
onJsonRead, (Json), (override));
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

class ProgressServiceListenerTest final : public ProgressServiceListener {
public:
  MOCK_METHOD(void, progressUpdate, (PB::ProgressStatus), (override));
};

class ThreadSchedulerMock final : public PBDev::ThreadScheduler {
public:
  explicit ThreadSchedulerMock(std::chrono::milliseconds waitTime)
      : mWaitTime(waitTime)
  {
  }
  ~ThreadSchedulerMock() = default;

  void post(std::function<void()> f) override { mQueue.enqueue(f); }

  void mainloop()
  {
    while (true) {
      auto f = mQueue.dequeue(mWaitTime);
      if (f) {
        f();
      }
      else {
        spdlog::info("ThreadSchedulerMock::mainloop() break");
        break;
      }
    }
  }

private:
  PB::TSQueue<std::function<void()>> mQueue;
  std::chrono::milliseconds          mWaitTime;
};

std::shared_ptr<PB::PlatformInfo> mockPlatformInfo();
std::shared_ptr<PB::PlatformInfo> mockPlatformInfo(Path installationPath,
                                                   Path locaStatePath);

std::shared_ptr<DatabaseService>
mockDatabaseService(std::shared_ptr<PB::PlatformInfo> platformInfo);

std::shared_ptr<ProjectSerializerService>
mockProjectSerializerService(std::shared_ptr<PB::PlatformInfo> platformInfo);

std::shared_ptr<DurableHashService>
mockDurableHashService(std::shared_ptr<DatabaseService> databaseService);
