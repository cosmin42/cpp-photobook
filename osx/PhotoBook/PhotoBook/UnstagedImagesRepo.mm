//
//  UnstagedImagesRepo.mm
//  PhotoBook
//
//  Created by Cosmin Mihai on 18.01.2025.
//

#include "UnstagedImagesRepo.h"

@implementation UnstagedImagesRepo
std::shared_ptr<PB::ImageMonitor> cppImageMonitor = nullptr;
- (id) initWithCpp:(std::shared_ptr<PB::ImageMonitor>)imageMonitor
{
    cppImageMonitor = imageMonitor;
    return self;
}
@end
