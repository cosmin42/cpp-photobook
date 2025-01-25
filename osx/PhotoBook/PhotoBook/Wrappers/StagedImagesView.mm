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
@end
