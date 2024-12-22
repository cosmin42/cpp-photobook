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
- (void)onMetadataUpdated:(NSString*)focusedName;
@end

@interface NoirListenerWrapperCLevel: NSObject
- (void)onNoirLutAdded;
- (void)onNoirError;
@end

// TODO: Fix inconsistent capita letters naming
@interface Photobook : NSObject
- (id)init;
- (void) setPhotobookListener:PhotobookListenerWrapperCLevel;
- (void) setNoirListener:NoirListenerWrapperCLevel;

- (void) startPhotobook;

- (NSString*) generateProjectName;
- (void) NewProject:(PaperSettings*)paperSettings;
- (void) RecallMetadata;
- (void) rename:(NSString*)oldName newName:(NSString*)newName;
- (void) remove:(NSString*)projectId;
- (void) unloadProject;
- (NSArray<ProjectMetadataEntry*>*) projectsList;
@end

#endif /* Photobook_h */
