//
//  Photobook.m
//  PhotoBook
//
//  Created by Cosmin Mihai on 09.06.2024.
//
#import <Foundation/Foundation.h>

#include <memory>

#include <pb/PhotoBook.h>
#include <pb/entities/LutIconInfo.h>

#include "Photobook.h"

@implementation PhotobookListenerWrapperCLevel
- (void)onProjectRead {
}

@end

@implementation NoirListenerWrapperCLevel
- (void)onNoirLutAdded{}
- (void)onNoirError{}
@end

class NoirListenerManaged final: public PB::NoirListener
{
public:
    explicit NoirListenerManaged(NoirListenerWrapperCLevel const & managedListener):mManagedListener(managedListener)
    {
        NSLog(@"Setting up Noir listener.");
    }
    ~NoirListenerManaged() = default;
    
    void onNoirLutAdded(PB::LutIconInfo iconInfo) override {}
    void onNoirError(PBDev::Error) {}
    
private:
    NoirListenerWrapperCLevel const& mManagedListener;
};

class PhotobookListenerManaged final: public PB::PhotobookListener
{
public:
    explicit PhotobookListenerManaged(PhotobookListenerWrapperCLevel const & managedListener):mManagedListener(managedListener)
    {
        NSLog(@"Setting up PBN listener.");
    }
    ~PhotobookListenerManaged() = default;
    void onProjectRead() override {
        [&mManagedListener onProjectRead];
    }
    void onMetadataUpdated(std::string targetName) override {
        NSString* managedFocusedName = [NSString stringWithUTF8String:targetName.c_str()];
        [&mManagedListener onMetadataUpdated:managedFocusedName];
    }
    void onPersistenceError(PBDev::Error) override {}
    void onExportFinished() override {}
    void onError(PBDev::Error) override {}
    void onStagedImageRemoved(std::vector<unsigned> removedIndexes) override {}
    void onMappingStarted(Path path) override {}
    void onMappingFinished(Path path) override {
        NSString* managedRoot = [NSString stringWithUTF8String:path.string().c_str()];
        [&mManagedListener onMappingFinished:managedRoot];
    }
    void onMappingAborted(Path path) override {}
    void onCollageThumbnailsCreated() override {}
    void onImageUpdated(Path root, int row, int index) override {}
    void post(std::function<void()> f) override {
        dispatch_async(dispatch_get_main_queue(), ^{
            f();
        });
    }
    void onCollageCreated(unsigned index, PB::GenericImagePtr newImage) override {}
    void onImageMapped(PBDev::ImageToPaperId id,
                       PB::GenericImagePtr       image) override {}
    void onProgressUpdate(PB::ProgressStatus status) override {}
    
    [[deprecated]]
    void onLutAdded(PB::LutIconInfo iconInfo) override {}
private:
    PhotobookListenerWrapperCLevel const& mManagedListener;
};


@implementation Photobook

NSString* installFolderPath = [[NSBundle mainBundle] resourcePath];
NSString* localFolderPath = [NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES) objectAtIndex:0];

Path nativeInstallFolderPath = [installFolderPath UTF8String];
Path nativeLocalFolderPath = [localFolderPath UTF8String];

std::shared_ptr<PB::Photobook> mPhotobook = nullptr;

PhotobookListenerManaged* mListener = nullptr;
NoirListenerManaged* mNoirListener = nullptr;

-(id)init {
    NSLog(@"Initializing photobook");
    NSLog(@"Local folder: %@", localFolderPath);
    NSLog(@"Install folder: %@", installFolderPath);
    
    mPhotobook = std::make_shared<PB::Photobook>(nativeLocalFolderPath, nativeInstallFolderPath, std::pair{1280, 720});
    return self;
}


- (void) setPhotobookListener:(PhotobookListenerWrapperCLevel const &)photobookListenerWrapperCLevel {
    if (mListener)
    {
        delete mListener;
    }
    mListener = new PhotobookListenerManaged(photobookListenerWrapperCLevel);
    mPhotobook->configure(mListener);
}

- (void) setNoirListener:(NoirListenerWrapperCLevel const &)noirListenerWrapperCLevel {
    // TODO: Add asset listener is null
    mNoirListener = new NoirListenerManaged(noirListenerWrapperCLevel);
    mPhotobook->configureNoirListener(mNoirListener);
}

- (void) startPhotobook {
    mPhotobook->startPhotobook();
}

- (NSString*) generateProjectName
{
    auto newProjectName = mPhotobook->projectManagementService()->newAlbumName();
    
    return [NSString stringWithUTF8String:newProjectName.c_str()];
}

- (void) NewProject:(PaperSettings*)paperSettings
{
    PB::PaperSettings nativePaperSettings = {(PB::PaperType)paperSettings.paperType, paperSettings.ppi, paperSettings.width, paperSettings.height};
    mPhotobook->projectManagementService()->newProject(nativePaperSettings);
}

- (void) RecallMetadata
{
    mPhotobook->projectManagementService()->recallMetadata();
}

- (NSArray<ProjectMetadataEntry*>*) projectsList
{
    NSMutableArray<ProjectMetadataEntry*>* result = [[NSMutableArray alloc] init];
    auto projectsList = mPhotobook->projectManagementService()->projectsList();
    for(auto& projectEntity: projectsList)
    {
        NSString* projectId = [NSString stringWithUTF8String:boost::uuids::to_string(std::get<0>(projectEntity)).c_str()];
        NSString* projectName = [NSString stringWithUTF8String:std::get<1>(projectEntity).c_str()];
        NSString* projectPath = [NSString stringWithUTF8String:std::get<2>(projectEntity).string().c_str()];
        ProjectMetadataEntry* managedProjectEntity = [[ProjectMetadataEntry alloc] init:projectId projectName:projectName projectPath:projectPath];
        [result addObject: managedProjectEntity];
    }
    return result;
}

- (void) unloadProject
{
    mPhotobook->unloadProject();
}

- (void) makeCollages
{
    mPhotobook->makeCollages();
}

- (ProjectManagementServiceWrapper*) projectManagementService
{
    auto projectManagementService = mPhotobook->projectManagementService();
    return [[ProjectManagementServiceWrapper alloc] initWithCpp: projectManagementService];
}

- (void) addImportFolder:(NSString*)root
{
    NSURL *url = [NSURL URLWithString:root];
    NSString *filePath = [url path];
    
    std::string nativeRoot = [filePath UTF8String];
    mPhotobook->addImportFolder(nativeRoot);
}

- (void) removeImportFolder:(NSString*)root
{
    NSURL *url = [NSURL URLWithString:root];
    NSString *filePath = [url path];
    
    std::string nativeRoot = [filePath UTF8String];
    mPhotobook->removeImportFolder(nativeRoot);
}

@end
