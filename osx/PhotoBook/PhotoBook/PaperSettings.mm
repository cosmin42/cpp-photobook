//
//  Untitled.swift
//  PhotoBook
//
//  Created by Cosmin Mihai on 01.11.2024.
//

#import <Foundation/Foundation.h>

#include "PaperSettings.h"

#include <pb/Config.h>
#include <pb/entities/PaperSettings.h>


@implementation PaperSettings
- (id) initWithCpp:(PB::PaperSettings) paperSettings
{
    self.paperType = (PaperType)paperSettings.type;
    self.width = paperSettings.width;
    self.height = paperSettings.height;
    self.ppi = paperSettings.ppi;
    return self;
}

- (id) init
{
    PB::PaperSettings defaultPaperSettings;
    self.paperType = (PaperType)defaultPaperSettings.type;
    self.width = defaultPaperSettings.width;
    self.height = defaultPaperSettings.height;
    self.ppi = defaultPaperSettings.ppi;
    return self;
}

+ (PaperSettings*) GetDefaultSettings: (PaperType)paperType
{
    switch (paperType)
    {
        case A4_Landscape:
            return [[PaperSettings alloc] initWithCpp:(PB::OneConfig::A4_LANDSCAPE_PAPER)];
        case A4_Portrait:
            return [[PaperSettings alloc] initWithCpp:(PB::OneConfig::A4_PORTRAIT_PAPER)];
        case A3_Landscape:
            return [[PaperSettings alloc] initWithCpp:(PB::OneConfig::A3_LANDSCAPE_PAPER)];
        case A3_Portrait:
            return [[PaperSettings alloc] initWithCpp:(PB::OneConfig::A3_PORTRAIT_PAPER)];
        case A5_Landscape:
            return [[PaperSettings alloc] initWithCpp:(PB::OneConfig::A5_LANDSCAPE_PAPER)];
        case A5_Portrait:
            return [[PaperSettings alloc] initWithCpp:(PB::OneConfig::A5_PORTRAIT_PAPER)];
        case Square_Paper:
            return [[PaperSettings alloc] initWithCpp:(PB::OneConfig::SQUARE_PAPER)];
        case None:
            break;
        case Custom:
            break;
    }
    
    NSAssert(false, @"Shouldn't get here");
    
    return [[PaperSettings alloc] init];
}

+ (PaperSettings*) GetDefaultSettingsByName: (NSString*)paperType
{
    auto nativePaperTypeName = [paperType UTF8String];
    PaperType managedPaperType = (PaperType)PB::deserializePaperType(nativePaperTypeName);
    return [PaperSettings GetDefaultSettings:managedPaperType];
}

@end
