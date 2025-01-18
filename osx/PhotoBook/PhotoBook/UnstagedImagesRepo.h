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

@interface UnstagedImagesRepo : NSObject
#if __cplusplus
- (id) initWithCpp:(PB::ImageMonitor&)imageMonitor;
#endif
@end

#endif /* UnstagedImagesRepo_h */
