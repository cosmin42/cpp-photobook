//
//  ProjectManagementService.mm
//  PhotoBook
//
//  Created by Cosmin Mihai on 22.12.2024.
//
#import <Foundation/Foundation.h>

#include "ProjectManagementService.h"

@implementation ProjectManagementServiceWrapper

std::shared_ptr<PB::Service::ProjectManagementService> projectManagementServiceCpp = nullptr;

- (id) initWithCpp:(std::shared_ptr<PB::Service::ProjectManagementService>)projectManagementService
{
    projectManagementServiceCpp = projectManagementService;
    return self;
}

- (void) removeByName:(NSString*)name
{
    std::string nativeName = [name UTF8String];
    projectManagementServiceCpp->deleteProject(nativeName);
}

- (void)rename:(NSString*)oldName newName:(NSString*)newName
{
    std::string nativeOldName = [oldName UTF8String];
    std::string nativeNewName = [newName UTF8String];
    projectManagementServiceCpp->renameProject(nativeOldName, nativeNewName);
}

- (PaperSettings*) paperSettings
{
    auto project = projectManagementServiceCpp->maybeLoadedProjectInfo();
    auto nativePaperSettings = project->value.paperSettings;
    
    return [[PaperSettings alloc] initWithCpp: nativePaperSettings];
}

- (UnstagedImagesRepo*) unstagedImagesRepo
{
    auto maybeProject = projectManagementServiceCpp->maybeLoadedProjectInfo();
    
    return [[UnstagedImagesRepo alloc] initWithCpp:maybeProject->value.imageMonitor()];
}

- (StagedImagesView*) stagedImages
{
    auto maybeProject = projectManagementServiceCpp->maybeLoadedProjectInfo();
    
    return [[StagedImagesView alloc] initWithCpp:maybeProject->value.stagedImages()];
}

- (NSArray<FrontendImage*>*) draftPhotoLine:(NSString*)thumbnailsPath;
{
    auto maybeProject = projectManagementServiceCpp->maybeLoadedProjectInfo();
    auto& nativeImages = maybeProject->value.draftImages();
    
    NSMutableArray<FrontendImage*>* result = [NSMutableArray new];
    
    for(auto i = 0; i < nativeImages.size(); i++)
    {
        auto image = nativeImages.at(i);
        [result addObject:[[FrontendImage alloc] initWithCpp:image projectRoot:thumbnailsPath]];
    }
    return result;
}

- (void) appendImage:(FrontendImage*)image
{
    auto nativeImage = [image unwrap];
    auto maybeProject = projectManagementServiceCpp->maybeLoadedProjectInfo();
    maybeProject->value.draftImages().push_back(nativeImage);
}

- (void) insertImage:(FrontendImage*)image atIndex:(unsigned)index
{
    auto nativeImage = [image unwrap];
    auto maybeProject = projectManagementServiceCpp->maybeLoadedProjectInfo();
    maybeProject->value.draftImages().insert(maybeProject->value.draftImages().begin() + index, nativeImage);
}

- (void) removeDraftImages:(NSArray<NSNumber*>*)indexes
{
    auto maybeProject = projectManagementServiceCpp->maybeLoadedProjectInfo();
    
    auto& images = maybeProject->value.draftImages();
    
    std::vector<unsigned> nativeIndexes;
    for (NSNumber* index in indexes)
    {
        nativeIndexes.push_back([index unsignedIntValue]);
    }
    
    std::sort(nativeIndexes.begin(), nativeIndexes.end(), std::greater<unsigned>());
    
    for(auto i : nativeIndexes)
    {
        images.erase(images.begin() + i);
    }
}


@end
