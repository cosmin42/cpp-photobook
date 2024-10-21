//
//  Photobook.h
//  PhotoBook
//
//  Created by Cosmin Mihai on 11.06.2024.
//

#ifndef Photobook_h
#define Photobook_h

@interface PhotobookListenerWrapperCLevel: NSObject
- (void)onProjectRead;
@end

@interface Photobook : NSObject
- (id)init;
- (void) startPhotobook;
- (void) setPhotobookListener:PhotobookListenerWrapperCLevel;
@end

#endif /* Photobook_h */
