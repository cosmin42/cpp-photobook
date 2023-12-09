#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <pb/ImageMonitor.h>

class MockImageMonitorListener final : public PB::ImageMonitorListener {
public:
  MOCK_METHOD(void, onImportFolderAdded, (), (override));
  MOCK_METHOD(void, onImportFolderRemoved, (unsigned index), (override));
  MOCK_METHOD(void, onCleared, (), (override));
};

class ImageMonitorVirtualImage : public PB::VirtualImage {
  MOCK_METHOD(PB::VirtualImageType, type, (), (const override));
};

TEST(TestImageMonitor, TestCreation)
{
  PB::ImageMonitor imageMonitor;

  auto listenerPtr = std::make_shared<MockImageMonitorListener>();

  imageMonitor.setListener(listenerPtr);
}

TEST(TestImageMonitor, TestEmpty)
{
  PB::ImageMonitor imageMonitor;

  auto listenerPtr = std::make_shared<MockImageMonitorListener>();

  imageMonitor.setListener(listenerPtr);

  EXPECT_CALL(*listenerPtr.get(), onCleared());

  imageMonitor.clear();
}

TEST(TestImageMonitor, TestAdding)
{
  PB::ImageMonitor imageMonitor;

  auto listenerPtr = std::make_shared<MockImageMonitorListener>();

  imageMonitor.setListener(listenerPtr);

  std::vector<std::shared_ptr<PB::VirtualImage>> newImageRow = {
      std::make_shared<ImageMonitorVirtualImage>(),
      std::make_shared<ImageMonitorVirtualImage>(),
      std::make_shared<ImageMonitorVirtualImage>(),
      std::make_shared<ImageMonitorVirtualImage>()};

  EXPECT_CALL(*listenerPtr.get(), onImportFolderAdded());

  imageMonitor.addRow(PB::Path("a/b"), newImageRow);

  EXPECT_EQ(imageMonitor.importListSize(), 1);

  EXPECT_EQ(imageMonitor.rowSize(0), 4);
}