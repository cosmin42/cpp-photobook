//
//  CollageItem.h
//  PhotoBook
//
//  Created by Cosmin Mihai on 25.01.2025.
//

//
//  FrontendImageResources.h
//  PhotoBook
//
//  Created by Cosmin Mihai on 18.01.2025.
//

#ifndef CollageItem_h
#define CollageItem_h

#import <Foundation/Foundation.h>

#if __cplusplus
#include <pb/entities/CollageTemplateInfo.h>
#endif

@interface CollageItem : NSObject
@property (nonatomic, strong) NSString* path;
@property (nonatomic, strong) NSString* name;
@property (nonatomic) unsigned imagesCount;
#if __cplusplus
- (id)initWithCpp:(PB::CollageTemplateInfo)collageTemplateInfo;
#endif
@end

#endif /* FrontendImageResources_h */
