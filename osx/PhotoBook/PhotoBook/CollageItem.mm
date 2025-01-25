//
//  CollageItem.mm
//  PhotoBook
//
//  Created by Cosmin Mihai on 25.01.2025.
//

#include "CollageItem.h"

@implementation CollageItem
- (id) initWithCpp:(PB::CollageTemplateInfo) collageTemplateInfo
{
    self.name = [NSString stringWithUTF8String:collageTemplateInfo.name.c_str()];
    self.path = [NSString stringWithUTF8String:collageTemplateInfo.path.c_str()];
    self.imagesCount = collageTemplateInfo.imageCount;
    return self;
}
@end
