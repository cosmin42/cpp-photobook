//
//  PaperSettings.g
//  PhotoBook
//
//  Created by Cosmin Mihai on 01.11.2024.
//

#ifndef PaperSettings_h
#define PaperSettings_h

// This must be in sync with PaperSettings from the library
typedef NS_ENUM(NSInteger, PaperType) {
    None,
    A4_Landscape,
    A4_Portrait,
    A5_Landscape,
    A5_Portrait,
    A3_Landscape,
    A3_Portrait,
    Custom,
    Square_Paper
};


@interface PaperSettings : NSObject
+ (PaperSettings*) GetDefaultSettings: (PaperType)paperType;

- (id)init;

@property PaperType paperType;
@property int       ppi;
@property int       width;
@property int       height;
@end

#endif /* PaperSettings_h */
