//
//  FrontendImage.h
//  PhotoBook
//
//  Created by Cosmin Mihai on 18.01.2025.
//

#ifndef FrontendImage_h
#define FrontendImage_h

#include "FrontendImageResources.h"

#if __cplusplus
#include <pb/entities/GenericImage.h>
#endif

typedef NS_ENUM(NSInteger, VirtualImageType) {
    VirtualImageType_None,
    VirtualImageType_Text,
    VirtualImageType_Regular,
    VirtualImageType_Test
};

@interface FrontendImage : NSObject
#if __cplusplus
- (id)initWithCpp:(PB::GenericImagePtr)genericImage;
#endif
- (VirtualImageType)imageType;
- (FrontendImageResources*)resources;
#if __cplusplus
- (PB::GenericImagePtr)unwrap;
#endif
@end


#endif /* FrontendImage_h */
