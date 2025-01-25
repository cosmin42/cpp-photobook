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

@interface StagedImagesView : NSObject
#if __cplusplus
- (id) initWithCpp:(std::shared_ptr<PB::StagedImages>)stagedImages;
#endif
@end

#endif /* StagedImagesView_h */
