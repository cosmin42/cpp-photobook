//
//  FrontendImage.mm
//  PhotoBook
//
//  Created by Cosmin Mihai on 18.01.2025.
//

#import <Foundation/Foundation.h>

#include "FrontendImage.h"

@implementation FrontendImage
{
    PB::GenericImagePtr cppImage;
}

- (id)initWithCpp:(PB::GenericImagePtr)genericImage
{
    cppImage = genericImage;
    return self;
}

- (VirtualImageType)imageType
{
    return VirtualImageType_None;
}

- (FrontendImageResources*)resources
{
    auto frontendImage = [FrontendImageResources alloc];
    frontendImage.full = [NSString stringWithUTF8String:cppImage->full().c_str()];
    frontendImage.medium = [NSString stringWithUTF8String:cppImage->medium().c_str()];
    frontendImage.small = [NSString stringWithUTF8String:cppImage->smaLL().c_str()];
    return frontendImage;
}

@end
