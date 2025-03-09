//
//  UnstagedImagesRepo.mm
//  PhotoBook
//
//  Created by Cosmin Mihai on 18.01.2025.
//

#include "UnstagedImagesRepo.h"

@implementation UnstagedImagesRepo
{
    std::shared_ptr<PB::ImageMonitor> cppImageMonitor;
}

- (id) initWithCpp:(std::shared_ptr<PB::ImageMonitor>)imageMonitor
{
    cppImageMonitor = imageMonitor;
    return self;
}

- (unsigned) rowSize:(unsigned)rowIndex
{
    return cppImageMonitor->rowSize(rowIndex);
}

- (FrontendImage*) image:(unsigned)row index:(unsigned)index thumbnailsPath:(NSString*)thumbnailsPath
{
    auto genericImage = cppImageMonitor->image(row, index);
    return [[FrontendImage alloc] initWithCpp:genericImage projectRoot:thumbnailsPath];
}

- (NSArray<MediaItem*>*) rowList
{
    auto rowList = cppImageMonitor->rowList();
    NSMutableArray<MediaItem*>* result = [NSMutableArray new];
    
    for(auto i = 0; i < rowList.size(); i++)
    {
        auto displayName = rowList.at(i).stem();
        NSString* path = [NSString stringWithUTF8String:rowList.at(i).string().c_str()];
        NSString* displayNameNative = [NSString stringWithUTF8String:displayName.string().c_str()];
        UInt32 imagesCount = cppImageMonitor->rowSize(i);
        MediaItem* mediaItem = [[MediaItem alloc] initWithCpp:path displayName:displayNameNative imagesCount:imagesCount];
        [result addObject:mediaItem];
    }
    return result;
}

- (void) removeRow:(unsigned)rowIndex
{
    cppImageMonitor->removeRow(rowIndex);
}
@end
