//
//  FrontendImage.mm
//  PhotoBook
//
//  Created by Cosmin Mihai on 18.01.2025.
//

#import <Foundation/Foundation.h>
#import <UniformTypeIdentifiers/UniformTypeIdentifiers.h>

#include <pb/entities/RegularImageV2.h>

#include "FrontendImage.h"

@implementation FrontendImage
{
    PB::GenericImagePtr cppImage;
    NSString* projectRoot;
    NSString* originalImageName;
}

- (id)initWithCpp:(PB::GenericImagePtr)genericImage projectRoot:(NSString*)path
{
    cppImage = genericImage;
    projectRoot = path;
    auto maybeRegularImage = std::dynamic_pointer_cast<PB::RegularImageV2>(genericImage);
    if (maybeRegularImage)
    {
        originalImageName = [NSString stringWithUTF8String:maybeRegularImage->original().filename().string().c_str()];
    }
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

    frontendImage.full = [projectRoot stringByAppendingPathComponent:frontendImage.full];
    frontendImage.medium = [projectRoot stringByAppendingPathComponent:frontendImage.medium];
    frontendImage.small = [projectRoot stringByAppendingPathComponent:frontendImage.small];
    return frontendImage;
}

- (PB::GenericImagePtr)unwrap
{
    return cppImage;
}

- (NSString*)maybeOriginalName
{
    return originalImageName;
}

@end
