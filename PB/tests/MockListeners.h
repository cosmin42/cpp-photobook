#pragma once

#include <regex>

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
  MOCK_METHOD(void, onProgressUpdate, (Path, int, int), (override));
  MOCK_METHOD(void, onExportProgressUpdate, (int, int), (override));
  MOCK_METHOD(void, onExportFinished, (), (override));
  MOCK_METHOD(void, onError, (PBDev::Error), (override));
  MOCK_METHOD(void, onUnstagedImageAdded,
              (Path rootPath, Path fullPath, Path mediumPath, Path smallPath,
               int position),
              (override));
  MOCK_METHOD(void, onStagedImageAdded,
              (std::vector<std::shared_ptr<PB::VirtualImage>> photos,
               int                                            index),
              (override));
  MOCK_METHOD(void, onStagedImageRemoved, (std::vector<unsigned>), (override));

  MOCK_METHOD(void, onMappingStarted, (Path), (override));
  MOCK_METHOD(void, onMappingFinished, (Path), (override));
  MOCK_METHOD(void, onMappingAborted, (Path), (override));

  MOCK_METHOD(void, post, (std::function<void()>), (override));
};