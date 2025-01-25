//
//  ProjectMetadataEntry.h
//  PhotoBook
//
//  Created by Cosmin Mihai on 03.11.2024.
//

#ifndef ProjectMetadataEntry_h
#define ProjectMetadataEntry_h


@interface ProjectMetadataEntry : NSObject

- (id) init: (NSString*)projectId projectName:(NSString*)projectName projectPath:(NSString*)projectPath;

@property NSString* identifier;
@property NSString* name;
@property NSString* path;
@end

#endif /* ProjectMetadataEntry_h */
