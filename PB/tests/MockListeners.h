#pragma once

#include <regex>
#include <string>

#include <boost/bimap/bimap.hpp>
#include <boost/uuid/uuid.hpp>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <pb/PhotoBook.h>

void clearProjectCache();

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
  MOCK_METHOD(void, onCleared, (), (override));
};

class TestDashboardListener final : public PB::DashboardListener {
public:
  MOCK_METHOD(void, onProjectsMetadataLoaded,
              (std::vector<PB::ProjectMetadata> v), (override));
  MOCK_METHOD(void, onProjectRead, (), (override));
};

class TestPhotobookListener final : public PB::PhotobookListener {
public:
  MOCK_METHOD(void, onProjectRead, (), (override));
  MOCK_METHOD(void, onProjectRenamed, (), (override));
  MOCK_METHOD(void, onMetadataUpdated, (), (override));
  MOCK_METHOD(void, onPersistenceError, (PBDev::Error), (override));
  MOCK_METHOD(void, onProgressUpdate, (int, int), (override));
  MOCK_METHOD(void, onExportProgressUpdate, (int, int), (override));
  MOCK_METHOD(void, onExportFinished, (), (override));
  MOCK_METHOD(void, onError, (PBDev::Error), (override));

  MOCK_METHOD(void, onStagedImageAdded,
              (std::vector<std::shared_ptr<PB::VirtualImage>> photos,
               int                                            index),
              (override));
  MOCK_METHOD(void, onStagedImageRemoved, (std::vector<unsigned>), (override));

  MOCK_METHOD(void, onMappingStarted, (Path), (override));
  MOCK_METHOD(void, onMappingFinished, (Path), (override));
  MOCK_METHOD(void, onMappingAborted, (Path), (override));

  MOCK_METHOD(void, onImageUpdated, (Path, int, int), (override));

  MOCK_METHOD(void, post, (std::function<void()>), (override));
};

class TestPersistenceProjectListener final
    : public PB::PersistenceProjectListener {
public:
  MOCK_METHOD(void, onProjectRead, (std::shared_ptr<PB::Project>), (override));
  MOCK_METHOD(void, onProjectPersistenceError, (PBDev::Error), (override));
};

typedef boost::bimaps::bimap<boost::uuids::uuid, std::string> BimapWorkaround;

class TestPersistenceMetadataListener final
    : public PB::PersistenceMetadataListener {
public:
  MOCK_METHOD(void, onMetadataRead, (BimapWorkaround), (override));
  MOCK_METHOD(void, onMetadataPersistenceError, (PBDev::Error), (override));
};
