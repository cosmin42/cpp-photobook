//
//  FrontendImage.mm
//  PhotoBook
//
//  Created by Cosmin Mihai on 18.01.2025.
//

#import <Foundation/Foundation.h>
#import <UniformTypeIdentifiers/UniformTypeIdentifiers.h>

#include "FrontendImage.h"

@implementation FrontendImage
{
    PB::GenericImagePtr cppImage;
    NSString* projectRoot;
}

- (id)initWithCpp:(PB::GenericImagePtr)genericImage projectRoot:(NSString*)path
{
    cppImage = genericImage;
    projectRoot = path;
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

@end
