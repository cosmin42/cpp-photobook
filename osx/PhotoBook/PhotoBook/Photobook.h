//
//  Photobook.h
//  PhotoBook
//
//  Created by Cosmin Mihai on 11.06.2024.
//

#ifndef Photobook_h
#define Photobook_h

#include "PaperSettings.h"
#include "ProjectMetadataEntry.h"

@interface PhotobookListenerWrapperCLevel: NSObject
- (void)onProjectRead;
- (void)onMetadataUpdated;
@end

// TODO: Fix inconsistent capita letters naming
@interface Photobook : NSObject
- (id)init;
- (void) setPhotobookListener:PhotobookListenerWrapperCLevel;

- (void) startPhotobook;

- (NSString*) generateProjectName;
- (void) NewProject:(NSString*)name paperSettings:(PaperSettings*)paperSettings;
- (void) RecallMetadata;
- (void) rename:(NSString*)oldName newName:(NSString*)newName;
- (void) remove:(NSString*)projectId;
- (NSArray<ProjectMetadataEntry*>*) projectsList;
@end

#endif /* Photobook_h */
