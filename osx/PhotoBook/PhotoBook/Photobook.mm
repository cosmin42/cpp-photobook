//
//  Photobook.m
//  PhotoBook
//
//  Created by Cosmin Mihai on 09.06.2024.
//

#include <pb/PhotoBook.h>

#include "Photobook.h"

#import <Foundation/NSString.h>

@implementation PhotobookListenerWrapperCLevel
- (id) init
{
    return self;
}

@end

class PhotobookListenerManaged final: public PB::PhotobookListener
{
public:
    explicit PhotobookListenerManaged(PhotobookListenerWrapperCLevel const & managedListener):mManagedListener(managedListener)
    {
    }
    
    ~PhotobookListenerManaged() = default;
    
    void onProjectRead() override {
        [&mManagedListener onProjectRead];
    }
    
    void onProjectRenamed() override {}
    
    void onMetadataUpdated() override {}
    
    void onPersistenceError(PBDev::Error) override {}
    
    void onExportFinished() override {}
    
    void onError(PBDev::Error) override {}
    
    void onStagedImageAdded(std::vector<std::shared_ptr<PB::VirtualImage>> photos, int index = -1) override {}
    
    void onStagedImageRemoved(std::vector<unsigned> removedIndexes) override {}
    
    void onMappingStarted(Path path) override {}
    
    void onMappingFinished(Path path) override {}
    
    void onMappingAborted(Path path) override {}
    
    void onImageUpdated(Path root, int row, int index) override {}
    
    void post(std::function<void()> f) override {}
    
    void onProgressUpdate(PB::ProgressInfo definedProgress, PB::ProgressInfo undefinedProgress) override {}
private:
    PhotobookListenerWrapperCLevel const& mManagedListener;
};

@implementation Photobook

//std::shared_ptr<PB::Photobook> mPhotobook = nullptr;

PhotobookListenerManaged* mListener = nullptr;

- (NSString*)GetAppSpecificFolder
{
    
    NSArray *paths = NSSearchPathForDirectoriesInDomains(NSApplicationSupportDirectory, NSUserDomainMask, YES);
    NSString *appSupportDirectory = [paths objectAtIndex:0];
    
    NSString* appName = @"PhotobookNoir";
    NSString* testX = @"x";
    
    // Append your application's specific directory name
    NSString * appSpecificDirectory;// = [NSString stringWithFormat:@"Hello World"];
    
    // Ensure the directory exists
    NSError *error = nil;
    if (![[NSFileManager defaultManager] fileExistsAtPath:appSpecificDirectory]) {
        [[NSFileManager defaultManager] createDirectoryAtPath:appSpecificDirectory withIntermediateDirectories:YES attributes:nil error:&error];
    }
    
    if (error) {
        NSLog(@"Failed to create Application Support directory: %@", error.localizedDescription);
        return nil;
    } else {
        return appSpecificDirectory;
    }
    
    return nil;
}

-(id)init {
    NSLog(@"Initializing photobook");
    NSString* appSpecificFolder = [self GetAppSpecificFolder];
    NSLog(@"Using the following App path: %@", appSpecificFolder);
    //mPhotobook = std::make_shared<PB::Photobook>();
    return self;
}


- (void) setPhotobookListener:(PhotobookListenerWrapperCLevel const &)photobookListenerWrapperCLevel {
    if (mListener)
    {
        delete mListener;
    }
    mListener = new PhotobookListenerManaged(photobookListenerWrapperCLevel);
    //mPhotobook->configure(mListener);
}
@end

@implementation XC
- (NSString *)concatenateString:(NSString *)str1 withString:(NSString *)str2 {
    [[NSString alloc] init];
    return [str1 stringByAppendingString:str2];

}
- (void) g
{
    //[concatenateString:@"1", withString:@"2"];
}
@end

