//
//  MediaItem.h
//  PhotoBook
//
//  Created by Cosmin Mihai on 08.03.2025.
//


#ifndef MediaItem_h
#define MediaItem_h

#import <Foundation/Foundation.h>

@interface MediaItem : NSObject
- (id) init: (NSString*)path displayName:(NSString*)displayName imagesCount:(UInt32)imagesCount;

#if __cplusplus
- (id) initWithCpp:(NSString*)path displayName:(NSString*)displayName imagesCount:(UInt32)imagesCount;
#endif

@property (nonatomic, strong) NSString* path;
@property (nonatomic, strong) NSString* displayName;
@property (nonatomic) UInt32 imagesCount;
@end

#endif
