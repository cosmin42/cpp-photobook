//
//  MediaItem.cpp
//  PhotoBook
//
//  Created by Cosmin Mihai on 08.03.2025.
//


#include "MediaItem.h"

@implementation MediaItem

- (id) init: (NSString*)path displayName:(NSString*)displayName imagesCount:(UInt32)imagesCount
{
    self = [super init];
    if (self) {
        self.path = path;
        self.displayName = displayName;
        self.imagesCount = imagesCount;
    }
    return self;
}

- (id) initWithCpp:(NSString*)path displayName:(NSString*)displayName imagesCount:(UInt32)imagesCount
{
    self = [super init];
    if (self) {
        self.path = path;
        self.displayName = displayName;
        self.imagesCount = imagesCount;
    }
    return self;
}

@end
