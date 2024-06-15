//
//  Photobook.h
//  PhotoBook
//
//  Created by Cosmin Mihai on 11.06.2024.
//

#ifndef Photobook_h
#define Photobook_h

#import <Foundation/Foundation.h>

@interface PhotobookListenerWrapperCLevel: NSObject
- (void)onProjectRead;
@end

@interface Photobook : NSObject
- (id)init;
- (void) setPhotobookListener:PhotobookListenerWrapperCLevel;
@end

@interface XC: NSObject
- (NSString *)concatenateString:(NSString *)str1 withString:(NSString *)str2;
- (void) g;
@end

#endif /* Photobook_h */
