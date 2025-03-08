//
//  StagedImagesView.h
//  PhotoBook
//
//  Created by Cosmin Mihai on 18.01.2025.
//

#ifndef StagedImagesView_h
#define StagedImagesView_h

#import <Foundation/Foundation.h>

#if __cplusplus
#include <pb/StagedImages.h>
#endif

#include "FrontendImage.h"

@interface StagedImagesView : NSObject
#if __cplusplus
- (id) initWithCpp:(std::shared_ptr<PB::StagedImages>)stagedImages;
#endif
- (void) addImages:(NSArray<FrontendImage*>*)images;
- (void) addImages:(NSArray<FrontendImage*>*)images atIndex:(unsigned)index;
- (void) removeImages:(NSArray<NSNumber*>*)indexes;
- (NSArray<FrontendImage*>*)images:(NSString*)thumbnailsPath;
@end

#endif /* StagedImagesView_h */
