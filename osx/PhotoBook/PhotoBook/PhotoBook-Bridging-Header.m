//
//  PhotoBook-Bridging-Header.m
//  PhotoBook
//
//  Created by Cosmin Mihai on 09.06.2024.
//

#import <Foundation/Foundation.h>
#include "PhotoBook-Bridging-Header.h"

#include <pb/PhotoBook.h>


@implementation Photobook

PB::Photobook mPhotobook("a", "b");

-(id)init {
    NSLog(@"Initializing photobook");
    mPhotobook.configure(std::pair<int, int>{3, 4});
    return self;
}
@end
