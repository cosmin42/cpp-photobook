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

- (FrontendImage*) image:(unsigned)row index:(unsigned)index
{
    auto genericImage = cppImageMonitor->image(row, index);
    return [[FrontendImage alloc] initWithCpp:genericImage];
}
@end
