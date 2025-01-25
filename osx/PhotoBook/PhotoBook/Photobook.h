//
//  Photobook.h
//  PhotoBook
//
//  Created by Cosmin Mihai on 11.06.2024.
//

#ifndef Photobook_h
#define Photobook_h

#include "PaperSettings.h"
#include "CollageItem.h"
#include "ProjectMetadataEntry.h"
#include "ProjectManagementService.h"

@interface PhotobookListenerWrapperCLevel: NSObject
- (void)onProjectRead;
- (void)onMetadataUpdated:(NSString*)focusedName;
- (void)onMappingFinished:(NSString*)root;
- (void)onImageUpdated:(NSString*)root row:(unsigned)row index:(unsigned)index;
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
- (void) unloadProject;
- (void) makeCollages;
- (NSArray<ProjectMetadataEntry*>*) projectsList;
- (ProjectManagementServiceWrapper*) projectManagementService;
- (void) addImportFolder:(NSString*)root;
- (void) removeImportFolder:(NSString*)root;
- (NSArray<CollageItem*>*) collageTemplatesThumbnailsList;
@end

#endif /* Photobook_h */
