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

PB::Photobook mPhotobook("a", "b");

PhotobookListenerManaged* mListener = nullptr;

-(id)init {
    NSLog(@"Initializing photobook");
    mPhotobook.configure(std::pair<int, int>{3, 4});
    return self;
}

-

- (void) setPhotobookListener:(PhotobookListenerWrapperCLevel const &)photobookListenerWrapperCLevel {
    if (mListener)
    {
        delete mListener;
    }
    mListener = new PhotobookListenerManaged(photobookListenerWrapperCLevel);
    mPhotobook.configure(mListener);
}
@end
