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

  imageMonitor.setListener(listenerPtr.get());
}

TEST(TestImageMonitor, TestEmpty)
{
  PB::ImageMonitor imageMonitor;

  auto listenerPtr = std::make_shared<MockImageMonitorListener>();

  imageMonitor.setListener(listenerPtr.get());

  EXPECT_CALL(*listenerPtr.get(), onCleared());

  imageMonitor.clear();
}

TEST(TestImageMonitor, TestAdding)
{
  PB::ImageMonitor imageMonitor;

  auto listenerPtr = std::make_shared<MockImageMonitorListener>();

  imageMonitor.setListener(listenerPtr.get());

  std::vector<std::shared_ptr<PB::VirtualImage>> newImageRow = {
      std::make_shared<ImageMonitorVirtualImage>(),
      std::make_shared<ImageMonitorVirtualImage>(),
      std::make_shared<ImageMonitorVirtualImage>(),
      std::make_shared<ImageMonitorVirtualImage>()};

  EXPECT_CALL(*listenerPtr.get(), onImportFolderAdded());

  imageMonitor.addRow(PB::Path("a/b"), newImageRow);

  EXPECT_EQ(imageMonitor.importListSize(), 1);

  EXPECT_EQ(imageMonitor.rowSize(0), 4);

  EXPECT_EQ(imageMonitor.rowIndex(PB::Path("a/b")), 0);

  EXPECT_EQ(imageMonitor.containsRow(PB::Path("a/b")), true);
  EXPECT_EQ(imageMonitor.containsRow(PB::Path("a/c")), false);

  EXPECT_EQ(imageMonitor.rowPath(0), PB::Path("a/b"));

  imageMonitor.addRow(PB::Path("a/b"), newImageRow);

  EXPECT_EQ(imageMonitor.importListSize(), 1);

  EXPECT_EQ(imageMonitor.rowSize(0), 4);

  EXPECT_EQ(imageMonitor.rowIndex(PB::Path("a/b")), 0);

  EXPECT_EQ(imageMonitor.containsRow(PB::Path("a/b")), true);
  EXPECT_EQ(imageMonitor.containsRow(PB::Path("a/c")), false);

  EXPECT_EQ(imageMonitor.rowPath(0), PB::Path("a/b"));
}

TEST(TestImageMonitor, TestRemoving)
{
  PB::ImageMonitor imageMonitor;

  auto listenerPtr = std::make_shared<MockImageMonitorListener>();

  imageMonitor.setListener(listenerPtr.get());

  std::vector<std::shared_ptr<PB::VirtualImage>> newImageRow = {
      std::make_shared<ImageMonitorVirtualImage>(),
      std::make_shared<ImageMonitorVirtualImage>(),
      std::make_shared<ImageMonitorVirtualImage>(),
      std::make_shared<ImageMonitorVirtualImage>()};

  EXPECT_CALL(*listenerPtr.get(), onImportFolderAdded());

  imageMonitor.addRow(PB::Path("a/b"), newImageRow);

  EXPECT_EQ(imageMonitor.importListSize(), 1);

  EXPECT_EQ(imageMonitor.rowSize(0), 4);

  EXPECT_EQ(imageMonitor.rowIndex(PB::Path("a/b")), 0);

  EXPECT_EQ(imageMonitor.containsRow(PB::Path("a/b")), true);
  EXPECT_EQ(imageMonitor.containsRow(PB::Path("a/c")), false);

  EXPECT_EQ(imageMonitor.rowPath(0), PB::Path("a/b"));

  std::vector<std::shared_ptr<PB::VirtualImage>> newImageRow2 = {
      std::make_shared<ImageMonitorVirtualImage>(),
      std::make_shared<ImageMonitorVirtualImage>()};

  EXPECT_CALL(*listenerPtr.get(), onImportFolderAdded());

  imageMonitor.addRow(PB::Path("b/c"), newImageRow2);

  EXPECT_EQ(imageMonitor.importListSize(), 2);

  EXPECT_EQ(imageMonitor.rowSize(0), 4);
  EXPECT_EQ(imageMonitor.rowSize(1), 2);

  EXPECT_EQ(imageMonitor.rowIndex(PB::Path("a/b")), 0);
  EXPECT_EQ(imageMonitor.rowIndex(PB::Path("b/c")), 1);

  EXPECT_EQ(imageMonitor.containsRow(PB::Path("a/b")), true);
  EXPECT_EQ(imageMonitor.containsRow(PB::Path("b/c")), true);
  EXPECT_EQ(imageMonitor.containsRow(PB::Path("a/c")), false);

  EXPECT_EQ(imageMonitor.rowPath(0), PB::Path("a/b"));
  EXPECT_EQ(imageMonitor.rowPath(1), PB::Path("b/c"));

  EXPECT_EQ(imageMonitor.isPending(0), true);
  EXPECT_EQ(imageMonitor.isPending(1), true);

  EXPECT_EQ(imageMonitor.isPending(PB::Path("a/b")), true);
  EXPECT_EQ(imageMonitor.isPending(PB::Path("b/c")), true);

  imageMonitor.completeRow(0);

  EXPECT_EQ(imageMonitor.isPending(0), false);
  EXPECT_EQ(imageMonitor.isPending(1), true);

  EXPECT_EQ(imageMonitor.isPending(PB::Path("a/b")), false);
  EXPECT_EQ(imageMonitor.isPending(PB::Path("b/c")), true);

  imageMonitor.completeRow(1);

  EXPECT_EQ(imageMonitor.isPending(0), false);
  EXPECT_EQ(imageMonitor.isPending(1), false);

  EXPECT_EQ(imageMonitor.isPending(PB::Path("a/b")), false);
  EXPECT_EQ(imageMonitor.isPending(PB::Path("b/c")), false);

}