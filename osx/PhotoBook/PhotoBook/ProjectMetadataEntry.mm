//
//  ProjectMetadataEntry.mm
//  PhotoBook
//
//  Created by Cosmin Mihai on 03.11.2024.
//


#import <Foundation/Foundation.h>

#include "ProjectMetadataEntry.h"

#include <boost/uuid/uuid_io.hpp>

#include <pb/util/Traits.h>

@implementation ProjectMetadataEntry
- (id) initWithCpp:(boost::uuids::uuid)projectId projectName:(std::string)name projectPath:(Path)projectPath
{
    self.projectId = [NSString stringWithUTF8String:boost::uuids::to_string(projectId).c_str()];
    self.name = [NSString stringWithUTF8String:name.c_str()];
    self.projectPath = [NSString stringWithUTF8String:projectPath.string().c_str()];
    return self;
}
@end
