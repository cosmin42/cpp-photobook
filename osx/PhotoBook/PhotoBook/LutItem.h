//
//  LutItem.h
//  PhotoBook
//
//  Created by Cosmin Mihai on 25.01.2025.
//

#ifndef LutItem_h
#define LutItem_h

#import <Foundation/Foundation.h>

#if __cplusplus
#include <pb/entities/LutIconInfo.h>
#endif

@interface LutItem : NSObject
@property (nonatomic, strong) NSString* path;
@property (nonatomic, strong) NSString* name;
#if __cplusplus
- (id)initWithCpp:(PB::LutIconInfo)lutIconInfo;
#endif
@end

#endif /* LutItem_h */
