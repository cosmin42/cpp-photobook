//
//  UnstagedImagesRepo.h
//  PhotoBook
//
//  Created by Cosmin Mihai on 18.01.2025.
//

#ifndef UnstagedImagesRepo_h
#define UnstagedImagesRepo_h

#import <Foundation/Foundation.h>

#if __cplusplus
#include <pb/ImageMonitor.h>
#endif

#include "FrontendImage.h"

@interface UnstagedImagesRepo : NSObject
#if __cplusplus
- (id) initWithCpp:(std::shared_ptr<PB::ImageMonitor>)imageMonitor;
#endif

- (unsigned) rowSize:(unsigned)rowIndex;
- (FrontendImage*) image:(unsigned)row index:(unsigned)index;
@end

#endif /* UnstagedImagesRepo_h */
