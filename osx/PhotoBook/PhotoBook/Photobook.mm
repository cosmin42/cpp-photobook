//
//  Photobook.m
//  PhotoBook
//
//  Created by Cosmin Mihai on 09.06.2024.
//
#import <Foundation/Foundation.h>

#include <pb/PhotoBook.h>

#include "Photobook.h"



@implementation PhotobookListenerWrapperCLevel
- (id) init
{
    return self;
}
- (void)onProjectRead {
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
    void onStagedImageRemoved(std::vector<unsigned> removedIndexes) override {}
    void onMappingStarted(Path path) override {}
    void onMappingFinished(Path path) override {}
    void onMappingAborted(Path path) override {}
    void onCollageThumbnailsCreated() override {}
    void onImageUpdated(Path root, int row, int index) override {}
    void post(std::function<void()> f) override {}
    void onCollageCreated(unsigned index, PB::GenericImagePtr newImage) override {}
    void onImageMapped(PBDev::ImageToPaperId id,
                       PB::GenericImagePtr       image) override {}
    void onProgressUpdate(PB::ProgressStatus status) override {}
    void onLutAdded(PB::LutIconInfo iconInfo) override {}
private:
    PhotobookListenerWrapperCLevel const& mManagedListener;
};


@implementation Photobook

NSString* installFolderPath = [[NSBundle mainBundle] resourcePath];
NSString* localFolderPath = [NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES) objectAtIndex:0];

Path nativeInstallFolderPath = [installFolderPath UTF8String];
Path nativeLocalFolderPath = [localFolderPath UTF8String];

PB::Photobook mPhotobook(nativeLocalFolderPath, nativeInstallFolderPath, {1280, 720});

PhotobookListenerManaged* mListener = nullptr;

-(id)init {
    NSLog(@"Initializing photobook");
    NSLog(@"Local folder: %@", localFolderPath);
    NSLog(@"Install folder: %@", installFolderPath);
    return self;
}

- (void) startPhotobook {
    mPhotobook.startPhotobook();
}

- (void) setPhotobookListener:(PhotobookListenerWrapperCLevel const &)photobookListenerWrapperCLevel {
    if (mListener)
    {
        delete mListener;
    }
    mListener = new PhotobookListenerManaged(photobookListenerWrapperCLevel);
    mPhotobook.configure(mListener);
}
@end
