//
//  ProjectMetadataEntry.mm
//  PhotoBook
//
//  Created by Cosmin Mihai on 03.11.2024.
//


#import <Foundation/Foundation.h>

#include "ProjectMetadataEntry.h"

#include <boost/uuid/uuid_io.hpp>

#include <pb/infra/Traits.h>

@implementation ProjectMetadataEntry
- (id) initWithCpp:(boost::uuids::uuid)projectId projectName:(std::string)name projectPath:(Path)projectPath
{
    self.identifier = [NSString stringWithUTF8String:boost::uuids::to_string(projectId).c_str()];
    self.name = [NSString stringWithUTF8String:name.c_str()];
    self.path = [NSString stringWithUTF8String:projectPath.string().c_str()];
    return self;
}

- (id) init: (NSString*)projectId projectName:(NSString*)projectName projectPath:(NSString*)projectPath
{
    self.identifier = projectId;
    self.name = projectName;
    self.path = projectPath;
    return self;
}

@end
