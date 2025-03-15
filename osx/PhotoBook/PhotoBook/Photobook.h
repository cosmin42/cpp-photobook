//
//  Photobook.h
//  PhotoBook
//
//  Created by Cosmin Mihai on 11.06.2024.
//

#ifndef Photobook_h
#define Photobook_h

#import <AppKit/AppKit.h>

#include "PaperSettings.h"
#include "CollageItem.h"
#include "LutItem.h"
#include "FrontendImage.h"
#include "ProjectMetadataEntry.h"
#include "ProjectManagementService.h"

@interface PhotobookListenerWrapperCLevel: NSObject
- (void)onProjectRead;
- (void)onMetadataUpdated:(NSString*)focusedName;
- (void)onMappingFinished:(NSString*)root imagesCount:(unsigned)imagesCount;
- (void)onImageUpdated:(NSString*)root row:(unsigned)row index:(unsigned)index;
- (void)onCollageThumbnailsCreated;
- (void)onImageMapped:(NSString*)imageId image:(FrontendImage*)image;
- (void)onImageCopied:(NSString*)imageId image:(FrontendImage*)image;
- (void)onCollageCreated:(FrontendImage*)image;
- (void)onLutAppliedInMemory:(NSString*)imageId image:(NSImage*)image;
- (void)onLutAppliedOnDiskInplace:(NSString*)imageId;
- (void)onLutAppliedOnDisk:(NSString*)imageId image:(FrontendImage*)image;
- (void)onError:(NSString*)error;
@end

@interface NoirListenerWrapperCLevel: NSObject
- (void)onNoirLutAdded:(LutItem*)item;
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
- (void) createCollage:(unsigned)collageIndex images:(NSArray<FrontendImage*>*)images;
- (NSArray<ProjectMetadataEntry*>*) projectsList;
- (ProjectManagementServiceWrapper*) projectManagementService;
- (void) addImportFolder:(NSString*)root;
- (void) removeImportFolder:(NSString*)root;
- (NSArray<CollageItem*>*) collageTemplatesThumbnailsList;
- (void) mapImagesToSPL:(NSDictionary<NSString*, FrontendImage*>*)images backgroundColors:(NSDictionary<NSString*, NSColor*>*)backgroundColors overlapTypes:(NSDictionary<NSString*, NSString*>*)overlapTypes;
- (void) copyImagesToDpl:(NSDictionary<NSString*, FrontendImage*>*)images;
- (NSString*) getThumbnailsPath;
- (void) exportAlbum:(NSString*)path name:(NSString*)name exportPdf:(BOOL)exportPdf exportPdfOptimized:(BOOL)exportPdfOptimized exportJpg:(BOOL)exportJpg;
- (NSString*) applyLuInMemory:(NSImage*)image lutIndex:(unsigned)lutIndex;
- (NSString*) getText:(NSString*)key;
- (void) loadProject:(NSString*)projectId;
- (void) saveProject;
- (void) applyTransformationOnDisk:(NSString*)transformationId lutIndex:(unsigned)lutIndex image:(FrontendImage*)image saturation:(double)saturation contrast:(double)contrast brightness:(double)brightness inplace:(BOOL)inplace;
@end

#endif /* Photobook_h */
