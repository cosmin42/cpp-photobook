#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <pb/ImageMonitor.h>

// TODO: Remove redundant listeners
class MockImageMonitorListener final : public PB::ImageMonitorListener {
public:
  MOCK_METHOD(void, onImportFolderAdded, (), (override));
  MOCK_METHOD(void, onImportFolderRemoved, (unsigned index), (override));
  MOCK_METHOD(void, onRefresh, (), (override));
  MOCK_METHOD(void, onCleared, (), (override));
};

class ImageMonitorVirtualImage : public PB::VirtualImage {
public:
  explicit ImageMonitorVirtualImage(Path path) : mPath(path) {}
  ~ImageMonitorVirtualImage() = default;
  MOCK_METHOD(PB::VirtualImageType, type, (), (const override));
  MOCK_METHOD(std::vector<Path>, resources, (), (const override));

  Path keyPath() const override { return mPath; }

private:
  Path mPath;
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

  auto img0 = std::make_shared<ImageMonitorVirtualImage>(Path("a/b/c.jpg"));
  auto img1 = std::make_shared<ImageMonitorVirtualImage>(Path("a/b/d.jpg"));
  auto img2 = std::make_shared<ImageMonitorVirtualImage>(Path("a/b/e.jpg"));
  auto img3 = std::make_shared<ImageMonitorVirtualImage>(Path("a/b/f.jpg"));

  std::vector<std::shared_ptr<PB::VirtualImage>> newImageRow = {img0, img1,
                                                                img2, img3};

  EXPECT_CALL(*listenerPtr.get(), onImportFolderAdded());

  imageMonitor.addRow(Path("a/b"), newImageRow);

  EXPECT_EQ(imageMonitor.importListSize(), 1);

  EXPECT_EQ(imageMonitor.rowSize(0), 4);

  EXPECT_EQ(imageMonitor.rowIndex(Path("a/b")), 0);

  EXPECT_EQ(imageMonitor.containsRow(Path("a/b")), true);
  EXPECT_EQ(imageMonitor.containsRow(Path("a/c")), false);

  EXPECT_EQ(imageMonitor.rowPath(0), Path("a/b"));

  imageMonitor.addRow(Path("a/b"), newImageRow);

  EXPECT_EQ(imageMonitor.importListSize(), 1);

  EXPECT_EQ(imageMonitor.rowSize(0), 4);

  EXPECT_EQ(imageMonitor.rowIndex(Path("a/b")), 0);

  EXPECT_EQ(imageMonitor.containsRow(Path("a/b")), true);
  EXPECT_EQ(imageMonitor.containsRow(Path("a/c")), false);

  EXPECT_EQ(imageMonitor.rowPath(0), Path("a/b"));
}

TEST(TestImageMonitor, TestRemoving)
{
  PB::ImageMonitor imageMonitor;

  auto listenerPtr = std::make_shared<MockImageMonitorListener>();

  imageMonitor.setListener(listenerPtr.get());

  auto img0 = std::make_shared<ImageMonitorVirtualImage>(Path("a/b/c.jpg"));
  auto img1 = std::make_shared<ImageMonitorVirtualImage>(Path("a/b/d.jpg"));
  auto img2 = std::make_shared<ImageMonitorVirtualImage>(Path("a/b/e.jpg"));
  auto img3 = std::make_shared<ImageMonitorVirtualImage>(Path("a/b/f.jpg"));

  std::vector<std::shared_ptr<PB::VirtualImage>> newImageRow = {img0, img1,
                                                                img2, img3};

  EXPECT_CALL(*listenerPtr.get(), onImportFolderAdded());

  imageMonitor.addRow(Path("a/b"), newImageRow);

  EXPECT_EQ(imageMonitor.importListSize(), 1);

  EXPECT_EQ(imageMonitor.rowSize(0), 4);

  EXPECT_EQ(imageMonitor.rowIndex(Path("a/b")), 0);

  EXPECT_EQ(imageMonitor.containsRow(Path("a/b")), true);
  EXPECT_EQ(imageMonitor.containsRow(Path("a/c")), false);

  EXPECT_EQ(imageMonitor.rowPath(0), Path("a/b"));

  std::vector<std::shared_ptr<PB::VirtualImage>> newImageRow2 = {
      std::make_shared<ImageMonitorVirtualImage>(Path("a/b/1.jpg")),
      std::make_shared<ImageMonitorVirtualImage>(Path("a/b/2.jpg"))};

  EXPECT_CALL(*listenerPtr.get(), onImportFolderAdded());

  imageMonitor.addRow(Path("b/c"), newImageRow2);

  EXPECT_EQ(imageMonitor.importListSize(), 2);

  EXPECT_EQ(imageMonitor.rowSize(0), 4);
  EXPECT_EQ(imageMonitor.rowSize(1), 2);

  EXPECT_EQ(imageMonitor.rowIndex(Path("a/b")), 0);
  EXPECT_EQ(imageMonitor.rowIndex(Path("b/c")), 1);

  EXPECT_EQ(imageMonitor.containsRow(Path("a/b")), true);
  EXPECT_EQ(imageMonitor.containsRow(Path("b/c")), true);
  EXPECT_EQ(imageMonitor.containsRow(Path("a/c")), false);

  EXPECT_EQ(imageMonitor.rowPath(0), Path("a/b"));
  EXPECT_EQ(imageMonitor.rowPath(1), Path("b/c"));

  EXPECT_EQ(imageMonitor.isPending(0), true);
  EXPECT_EQ(imageMonitor.isPending(1), true);

  EXPECT_EQ(imageMonitor.isPending(Path("a/b")), true);
  EXPECT_EQ(imageMonitor.isPending(Path("b/c")), true);

  imageMonitor.completeRow(0);

  EXPECT_EQ(imageMonitor.isPending(0), false);
  EXPECT_EQ(imageMonitor.isPending(1), true);

  EXPECT_EQ(imageMonitor.isPending(Path("a/b")), false);
  EXPECT_EQ(imageMonitor.isPending(Path("b/c")), true);

  imageMonitor.completeRow(1);

  EXPECT_EQ(imageMonitor.isPending(0), false);
  EXPECT_EQ(imageMonitor.isPending(1), false);

  EXPECT_EQ(imageMonitor.isPending(Path("a/b")), false);
  EXPECT_EQ(imageMonitor.isPending(Path("b/c")), false);
}