//
//  ProjectManagementService.h
//  PhotoBook
//
//  Created by Cosmin Mihai on 22.12.2024.
//

#ifndef ProjectManagementService_h
#define ProjectManagementService_h

#if __cplusplus
#include <pb/services/ProjectManagementService.h>
#endif

#include "PaperSettings.h"
#include "UnstagedImagesRepo.h"
#include "StagedImagesView.h"

@interface ProjectManagementServiceWrapper : NSObject
#if __cplusplus
- (id) initWithCpp:(std::shared_ptr<PB::Service::ProjectManagementService>)projectManagementService;
#endif
- (void)removeByName:(NSString*)name;
- (void)rename:(NSString*)oldName newName:(NSString*)newName;

- (PaperSettings*) paperSettings;

- (UnstagedImagesRepo*) unstagedImagesRepo;
- (StagedImagesView*) stagedImages;

@end

#endif
