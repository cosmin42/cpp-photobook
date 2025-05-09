//
//  Photobook.h
//  PhotoBook
//
//  Created by Cosmin Mihai on 11.06.2024.
//

#ifndef Photobook_h
#define Photobook_h

#if TARGET_OS_OSX
#import <AppKit/AppKit.h>
#else
#import <UIKit/UIKit.h>
#endif


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
- (void)onSearchingFinished:(NSString*)importedFolderPath placeholders:(NSDictionary<NSString*, FrontendImage*>*)placeholders;
- (void)onImageUpdated:(NSString*)root row:(unsigned)row index:(unsigned)index;
- (void)onCollageThumbnailsCreated;
- (void)onImageMapped:(NSString*)imageId image:(FrontendImage*)image;
- (void)onImageCopied:(NSString*)imageId image:(FrontendImage*)image;
- (void)onCollageCreated:(FrontendImage*)image;
#if TARGET_OS_OSX
- (void)onLutAppliedInMemory:(NSString*)imageId image:(NSImage*)image;
#else
- (void)onLutAppliedInMemory:(NSString*)imageId image:(UIImage*)image;
#endif
- (void)onLutAppliedOnDiskInplace:(NSString*)imageId;
- (void)onLutAppliedOnDisk:(NSString*)imageId image:(FrontendImage*)image;
- (void)onEffectsApplied:(NSString*)imageId image:(FrontendImage*)image;
- (void)onEffectsAppliedInplace:(NSString*)imageId;
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
- (void) addImportImages:(NSArray<NSString*>*)paths;
- (void) removeImportFolder:(NSString*)root;
- (NSArray<CollageItem*>*) collageTemplatesThumbnailsList;
#if TARGET_OS_OSX
- (void) mapImagesToSPL:(NSDictionary<NSString*, FrontendImage*>*)images backgroundColors:(NSDictionary<NSString*, NSColor*>*)backgroundColors overlapTypes:(NSDictionary<NSString*, NSString*>*)overlapTypes;
#else
- (void) mapImagesToSPL:(NSDictionary<NSString*, FrontendImage*>*)images backgroundColors:(NSDictionary<NSString*, UIColor*>*)backgroundColors overlapTypes:(NSDictionary<NSString*, NSString*>*)overlapTypes;
#endif
- (void) copyImagesToDpl:(NSDictionary<NSString*, FrontendImage*>*)images;
- (NSString*) getThumbnailsPath;
- (void) exportAlbum:(NSString*)path name:(NSString*)name exportPdf:(BOOL)exportPdf exportPdfOptimized:(BOOL)exportPdfOptimized exportJpg:(BOOL)exportJpg;
#if TARGET_OS_OSX
- (NSString*) applyLuInMemory:(NSImage*)image lutIndex:(unsigned)lutIndex;
#else
- (NSString*) applyLuInMemory:(UIImage*)image lutIndex:(unsigned)lutIndex;
#endif
- (NSString*) getText:(NSString*)key;
- (void) loadProject:(NSString*)projectId;
- (void) saveProject;
- (void) applyTransformationOnDisk:(NSString*)transformationId lutIndex:(unsigned)lutIndex image:(FrontendImage*)image inplace:(BOOL)inplace;
- (void) applyEffects:(NSString*)effectIdStr image:(FrontendImage*)image saturation:(double)saturation brightness:(double)brightness contrast:(double)contrast;
- (void) applyEffectsInPlace:(NSString*)effectIdStr image:(FrontendImage*)image saturation:(double)saturation brightness:(double)brightness contrast:(double)contrast;
- (void) subscribeByEmail:(NSString*)email apiKey:(NSString*)apiKey;
@end

#endif /* Photobook_h */
