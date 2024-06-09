//
//  PhotoBook-Bridging-Header.m
//  PhotoBook
//
//  Created by Cosmin Mihai on 09.06.2024.
//

#import <Foundation/Foundation.h>
#include "PhotoBook-Bridging-Header.h"

#include <pb/PhotoBook.h>


@implementation ExampleObjC
- (void)doSomething {
    PBDev::Path p = "a/b/c";
    NSLog(@"Doing something in Objective-C %s", p.string().c_str());
}
@end
