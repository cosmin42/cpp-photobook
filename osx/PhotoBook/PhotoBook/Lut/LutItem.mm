//
//  LutItem.mm
//  PhotoBook
//
//  Created by Cosmin Mihai on 25.01.2025.
//

#include "LutItem.h"

@implementation LutItem
- (id)initWithCpp:(PB::LutIconInfo)lutIconInfo;
{
    self.name = [NSString stringWithUTF8String:lutIconInfo.name.c_str()];
    self.path = [NSString stringWithUTF8String:lutIconInfo.path.c_str()];
    return self;
}
@end
