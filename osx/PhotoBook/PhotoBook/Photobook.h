//
//  Photobook.h
//  PhotoBook
//
//  Created by Cosmin Mihai on 11.06.2024.
//

#ifndef Photobook_h
#define Photobook_h

#include "PaperSettings.h"

@interface PhotobookListenerWrapperCLevel: NSObject
- (void)onProjectRead;
- (void)onMetadataUpdated;
@end

@interface Photobook : NSObject
- (id)init;
- (void) setPhotobookListener:PhotobookListenerWrapperCLevel;

- (void) startPhotobook;

- (NSString*) generateProjectName;
- (void) NewProject:(NSString*)name paperSettings:(PaperSettings*)paperSettings;
- (void) RecallMetadata;

@end

#endif /* Photobook_h */
