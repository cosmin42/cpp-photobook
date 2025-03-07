#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <pb/Config.h>
#include <pb/StagedImages.h>
#include <pb/image/ImageFactory.h>



TEST(TestStagedImages, AddPictures)
{
  /*
  std::shared_ptr<PB::StagedImages> stagedImages =
      std::make_shared<PB::StagedImages>();

  std::shared_ptr<TestStagedImagesListener> listener =
      std::make_shared<TestStagedImagesListener>();

  stagedImages->setListener(listener.get());

  std::vector<std::shared_ptr<PB::VirtualImage>> pictures;

  stagedImages->addPictures(pictures);

  std::shared_ptr<PB::VirtualImage> image0 =
      std::make_shared<TestStagedImagesVirtualImage>(PB::ImageFactory::inst().defaultImageFrontend());

  std::shared_ptr<PB::VirtualImage> image1 =
      std::make_shared<TestStagedImagesVirtualImage>(
          PB::ImageFactory::inst().defaultImageFrontend());

  pictures.push_back(image0);
  pictures.push_back(image1);

  EXPECT_CALL(*listener.get(), onPicturesAdded(0, 2));

  stagedImages->addPictures(pictures);

  auto const &retrievedPictures = stagedImages->stagedPhotos();

  EXPECT_EQ(retrievedPictures.size(), 2);

  std::shared_ptr<PB::VirtualImage> image2 =
      std::make_shared<TestStagedImagesVirtualImage>(
          PB::ImageFactory::inst().defaultImageFrontend());
  std::shared_ptr<PB::VirtualImage> image3 =
      std::make_shared<TestStagedImagesVirtualImage>(
          PB::ImageFactory::inst().defaultImageFrontend());
  std::shared_ptr<PB::VirtualImage> image4 =
      std::make_shared<TestStagedImagesVirtualImage>(
          PB::ImageFactory::inst().defaultImageFrontend());
  std::shared_ptr<PB::VirtualImage> image5 =
      std::make_shared<TestStagedImagesVirtualImage>(
          PB::ImageFactory::inst().defaultImageFrontend());
  std::shared_ptr<PB::VirtualImage> image6 =
      std::make_shared<TestStagedImagesVirtualImage>(
          PB::ImageFactory::inst().defaultImageFrontend());

  std::vector<std::shared_ptr<PB::VirtualImage>> otherPictures;

  otherPictures.push_back(image2);
  otherPictures.push_back(image3);
  otherPictures.push_back(image4);
  otherPictures.push_back(image5);
  otherPictures.push_back(image6);

  EXPECT_CALL(*listener.get(), onPicturesAdded(2, 5));

  stagedImages->addPictures(otherPictures);

  EXPECT_CALL(*listener.get(), onPictureRemoved(std::vector<unsigned>{}));

  stagedImages->removePicture({});

  EXPECT_CALL(*listener.get(), onPictureRemoved(std::vector<unsigned>{1}));

  stagedImages->removePicture({1});

  EXPECT_CALL(*listener.get(),
              onPictureRemoved(std::vector<unsigned>{0, 1, 2, 3, 4, 5}));

  stagedImages->clear();
  */
}