//
//  StagedImagesView.mm
//  PhotoBook
//
//  Created by Cosmin Mihai on 18.01.2025.
//

#include "StagedImagesView.h"

@implementation StagedImagesView
std::shared_ptr<PB::StagedImages> cppStagedImages = nullptr;
- (id) initWithCpp:(std::shared_ptr<PB::StagedImages>)stagedImages
{
    cppStagedImages = stagedImages;
    return self;
}

- (void) addImages:(NSArray<FrontendImage*>*)images
{
    std::vector<PB::GenericImagePtr> nativeImages;
    for (FrontendImage* image in images)
    {
        auto unwrapped = [image unwrap];
        nativeImages.push_back(unwrapped);
    }
    cppStagedImages->addPictures(nativeImages);
}

- (void) addImages:(NSArray<FrontendImage*>*)images atIndex:(unsigned)index
{
    std::vector<PB::GenericImagePtr> nativeImages;
    for (FrontendImage* image in images)
    {
        auto unwrapped = [image unwrap];
        nativeImages.push_back(unwrapped);
        
    }
    cppStagedImages->addPictures(nativeImages, index);
}

- (void) removeImages:(NSArray<NSNumber*>*)indexes
{
    std::vector<unsigned> nativeIndexes;
    for (NSNumber* index in indexes)
    {
        nativeIndexes.push_back([index unsignedIntValue]);
    }
    cppStagedImages->removePicture(nativeIndexes);
}

- (NSArray<FrontendImage*>*)images:(NSString*)thumbnailsPath;
{
    auto nativeImages = cppStagedImages->stagedPhotos();
    NSMutableArray<FrontendImage*>* result = [NSMutableArray new];
    
    for(auto i = 0; i < nativeImages.size(); i++)
    {
        auto image = nativeImages.at(i);
        [result addObject:[[FrontendImage alloc] initWithCpp:image projectRoot:thumbnailsPath]];
    }
    
    return result;
}

@end
