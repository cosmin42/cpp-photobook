//
//  ProjectManagementService.h
//  PhotoBook
//
//  Created by Cosmin Mihai on 22.12.2024.
//

#ifndef ProjectManagementService_h
#define ProjectManagementService_h

@interface ProjectManagementServiceWrapper : NSObject
- (void)removeByName:(NSString*)name;
- (void)rename:(NSString*)oldName newName:(NSString*)newName;
@end

#endif
