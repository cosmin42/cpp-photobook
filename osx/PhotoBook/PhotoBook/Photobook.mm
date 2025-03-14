//
//  Photobook.m
//  PhotoBook
//
//  Created by Cosmin Mihai on 09.06.2024.
//
#import <Foundation/Foundation.h>

#include <memory>
#include <unordered_map>

#include <pb/PhotoBook.h>
#include <pb/entities/LutIconInfo.h>

#include "Photobook.h"
#include "LutItem.h"

@implementation PhotobookListenerWrapperCLevel

@end

@implementation NoirListenerWrapperCLevel
@end

class NoirListenerManaged final: public PB::NoirListener
{
public:
    explicit NoirListenerManaged(NoirListenerWrapperCLevel const & managedListener):mManagedListener(managedListener)
    {
        NSLog(@"Setting up Noir listener.");
    }
    ~NoirListenerManaged() = default;
    
    void onNoirLutAdded(PB::LutIconInfo iconInfo) override {
        auto item = [[LutItem alloc] initWithCpp:iconInfo];
        [&mManagedListener onNoirLutAdded:item];
    }
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
    void onError(PBDev::Error error) override {
        NSString* managedError = [NSString stringWithUTF8String:error.description().c_str()];
        [&mManagedListener onError:managedError];
    }
    void onStagedImageRemoved(std::vector<unsigned> removedIndexes) override {}
    void onMappingStarted(Path path) override {}
    void onMappingFinished(Path path, unsigned imagesCount) override {
        NSString* managedRoot = [NSString stringWithUTF8String:path.string().c_str()];
        [&mManagedListener onMappingFinished:managedRoot imagesCount:imagesCount];
    }
    void onMappingAborted(Path path) override {}
    void onCollageThumbnailsCreated() override {
        [&mManagedListener onCollageThumbnailsCreated];
    }
    void onImageUpdated(Path root, int row, int index) override {
        NSString* managedRoot = [NSString stringWithUTF8String:root.string().c_str()];
        [&mManagedListener onImageUpdated:managedRoot row:row index:index];
    }
    void post(std::function<void()> f) override {
        dispatch_async(dispatch_get_main_queue(), ^{
            f();
        });
    }
    void onCollageCreated(PB::GenericImagePtr newImage, Path thumbnailsLocation) override {
        NSString* managedThumbnailsLocation = [NSString stringWithUTF8String:thumbnailsLocation.string().c_str()];
        auto managedImage = [[FrontendImage alloc] initWithCpp:newImage projectRoot:managedThumbnailsLocation];
        [&mManagedListener onCollageCreated:managedImage];
    }
    void onImageMapped(PBDev::ImageToPaperId imageId,
                       PB::GenericImagePtr       image, Path thumbnailsLocation) override {
        std::string imageIdStr = boost::uuids::to_string(*imageId);
        NSString* managedImageId = [NSString stringWithUTF8String:imageIdStr.c_str()];
        
        NSString* managedThumbnailsLocation = [NSString stringWithUTF8String:thumbnailsLocation.string().c_str()];
        
        auto managedImage = [[FrontendImage alloc] initWithCpp:image projectRoot:managedThumbnailsLocation];
        [&mManagedListener onImageMapped:managedImageId image:managedImage];
    }
    
    void onImageCopied(PBDev::ImageToPaperId imageId, PB::GenericImagePtr image, Path thumbnailsLocation) override {
        std::string imageIdStr = boost::uuids::to_string(*imageId);
        NSString* managedImageId = [NSString stringWithUTF8String:imageIdStr.c_str()];
        
        NSString* managedThumbnailsLocation = [NSString stringWithUTF8String:thumbnailsLocation.string().c_str()];
        
        auto managedImage = [[FrontendImage alloc] initWithCpp:image projectRoot:managedThumbnailsLocation];
        [&mManagedListener onImageCopied:managedImageId image:managedImage];
    }

    
    void onProgressUpdate(PB::ProgressStatus status) override {}
    
    [[deprecated]]
    void onLutAdded(PB::LutIconInfo iconInfo) override {}
    
    void onLutApplied(PBDev::LutApplicationId, PB::GenericImagePtr,
                      Path thumbnailsLocation) {}
    
    void onLutAppliedInMemory(PBDev::LutApplicationId imageId,
                              std::shared_ptr<cv::Mat> image) override {
        std::string imageIdStr = boost::uuids::to_string(*imageId);
        NSString* managedImageId = [NSString stringWithUTF8String:imageIdStr.c_str()];
        NSImage* managedImage = MatToNSImage(*image);
        [&mManagedListener onLutAppliedInMemory: managedImageId image:managedImage];
    }

    void onLutAppliedOnDiskInplace(PBDev::LutApplicationId applicationId) override
    {
        std::string imageIdStr = boost::uuids::to_string(*applicationId);
        NSString* managedImageId = [NSString stringWithUTF8String:imageIdStr.c_str()];
        
        [&mManagedListener onLutAppliedOnDiskInplace: managedImageId];
    }
    
private:
    PhotobookListenerWrapperCLevel const& mManagedListener;
    
    CGImageRef MatToCGImage(const cv::Mat& mat) {
        cv::Mat tempMat;
        if (mat.channels() == 1) {
            cv::cvtColor(mat, tempMat, cv::COLOR_GRAY2RGBA);
        } else if (mat.channels() == 3) {
            cv::cvtColor(mat, tempMat, cv::COLOR_BGR2RGBA);
        } else if (mat.channels() == 4) {
            tempMat = mat.clone();
        } else {
            return nil;
        }
        
        size_t width = tempMat.cols;
        size_t height = tempMat.rows;
        size_t bytesPerRow = tempMat.step[0];
        
        CGColorSpaceRef colorSpace = CGColorSpaceCreateWithName(kCGColorSpaceSRGB);
        CGContextRef context = CGBitmapContextCreate(tempMat.data, width, height, 8, bytesPerRow, colorSpace, kCGImageAlphaPremultipliedLast | kCGBitmapByteOrderDefault);
        CGImageRef imageRef = CGBitmapContextCreateImage(context);
        
        CGContextRelease(context);
        CGColorSpaceRelease(colorSpace);
        
        return imageRef;
    }
    
    NSImage* MatToNSImage(const cv::Mat& mat) {
        CGImageRef imageRef = MatToCGImage(mat);
        if (!imageRef) return nil;
        
        NSImage* image = [[NSImage alloc] initWithCGImage:imageRef size:NSMakeSize(mat.cols, mat.rows)];
        CGImageRelease(imageRef);
        
        return image;
    }
    
    
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

- (void) createCollage:(unsigned)collageIndex images:(NSArray<FrontendImage*>*)images
{
    std::vector<PB::GenericImagePtr> nativeImages;
    for (NSUInteger i = 0; i < [images count]; i++)
    {
        auto nativeImage = [[images objectAtIndex:i] unwrap];
        nativeImages.push_back(nativeImage);
    }
    mPhotobook->collageService()->combineImages(collageIndex, nativeImages);
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

- (NSArray<CollageItem*>*) collageTemplatesThumbnailsList
{
    auto list = [[NSMutableArray<CollageItem*> alloc] init];
    auto nativeList = mPhotobook->collageService()->getTemplatesPaths();
    for(auto& item: nativeList)
    {
        CollageItem* managedItem = [[CollageItem alloc] initWithCpp:item];
        [list addObject:managedItem];
    }
    return [list copy];
}

PB::Geometry::OverlapType overlayTypeFromString(NSString* overlayType)
{
    if ([overlayType isEqualToString:@"Fit"])
    {
        return PB::Geometry::OverlapType::Inscribed;
    }
    else if ([overlayType isEqualToString:@"Fill"])
    {
        return PB::Geometry::OverlapType::Circumscribed;
    }
    else
    {
        PBDev::basicAssert(false);
    }
    return PB::Geometry::OverlapType::Inscribed;
}

- (void) mapImagesToSPL:(NSDictionary<NSString*, FrontendImage*>*)images backgroundColors:(NSDictionary<NSString*, NSColor*>*)backgroundColors overlapTypes:(NSDictionary<NSString*, NSString*>*)overlapTypes;
{
    auto imageToPaperService = mPhotobook->imageToPaperService();
    
    std::unordered_map<PBDev::ImageToPaperId, PB::ImageToPaperData,
    boost::hash<PBDev::ImageToPaperId>>
    backendMap;
    
    for (NSString *key in images) {
        std::string uuidStr = [key UTF8String];
        try {
            boost::uuids::string_generator gen;
            boost::uuids::uuid nativeUuid = gen(uuidStr);
            
            PBDev::ImageToPaperId imageId = PBDev::ImageToPaperId(nativeUuid);
            
            cv::Scalar backgroundColor = {backgroundColors[key].blueComponent * 255, backgroundColors[key].greenComponent * 255, backgroundColors[key].redComponent * 255};
            
            PB::Geometry::OverlapType overlapType = overlayTypeFromString(overlapTypes[key]);
            
            backendMap[imageId] = {[images[key] unwrap], backgroundColor, overlapType};
            
        } catch (const std::exception& e) {
        }
    }
    
    imageToPaperService->map(PBDev::ImageToPaperServiceId(PB::RuntimeUUID::newUUID()), backendMap);
}

- (void) copyImagesToDpl:(NSDictionary<NSString*, FrontendImage*>*)images
{
    auto imageToPaperService = mPhotobook->imageToPaperService();
    
    std::unordered_map<PBDev::ImageToPaperId, PB::GenericImagePtr,
    boost::hash<PBDev::ImageToPaperId>>
    backendMap;
    
    for (NSString *key in images) {
        std::string uuidStr = [key UTF8String];
        try {
            boost::uuids::string_generator gen;
            boost::uuids::uuid nativeUuid = gen(uuidStr);
            
            PBDev::ImageToPaperId imageId = PBDev::ImageToPaperId(nativeUuid);

            backendMap[imageId] = [images[key] unwrap];
            
        } catch (const std::exception& e) {
        }
    }

    imageToPaperService->copyImages(PBDev::ImageToPaperServiceId(PB::RuntimeUUID::newUUID()), backendMap);
}

- (NSString*) getThumbnailsPath
{
    auto maybeProject =
    mPhotobook->projectManagementService()->maybeLoadedProjectInfo();
    PBDev::basicAssert(maybeProject != nullptr);
    auto nativeThumbnailsPath = mPhotobook->platformInfo()->projectSupportFolder(maybeProject->first) / "thumbnail-images";
    return [NSString stringWithUTF8String:nativeThumbnailsPath.string().c_str()];
}

- (void) exportAlbum:(NSString*)path name:(NSString*)name exportPdf:(BOOL)exportPdf exportPdfOptimized:(BOOL)exportPdfOptimized exportJpg:(BOOL)exportJpg
{
    std::string nativePath = [path UTF8String];
    std::string nativeName = [name UTF8String];
    if (exportPdf)
    {
        mPhotobook->exportService()->exportPDFLibharu(nativeName, nativePath);
    }
    if (exportPdfOptimized)
    {
        mPhotobook->exportService()->exportPDFAlbum(nativeName, nativePath);
    }
    if (exportJpg)
    {
        mPhotobook->exportService()->exportJPGAlbum(nativeName, nativePath);
    }
}

cv::Mat NSImageToMat(NSImage *image) {
    if (!image) return cv::Mat();
    
    // Convert NSImage to NSBitmapImageRep
    CGImageRef cgImage = [image CGImageForProposedRect:nil context:nil hints:nil];
    if (!cgImage) return cv::Mat();
    
    NSUInteger width = CGImageGetWidth(cgImage);
    NSUInteger height = CGImageGetHeight(cgImage);
    
    cv::Mat mat(height, width, CV_8UC4); // OpenCV Mat with 4 channels (RGBA)
    
    // Create a CGContext to draw the image
    CGContextRef context = CGBitmapContextCreate(mat.data, width, height, 8, mat.step[0],
                                                 CGImageGetColorSpace(cgImage),
                                                 kCGImageAlphaPremultipliedLast | kCGBitmapByteOrderDefault);
    
    if (!context) return cv::Mat();
    
    CGContextDrawImage(context, CGRectMake(0, 0, width, height), cgImage);
    CGContextRelease(context);
    
    return mat;
}

- (NSString*) applyLuInMemory:(NSImage*)image lutIndex:(unsigned)lutIndex
{
    PBDev::LutApplicationId lutId = PBDev::LutApplicationId(PB::RuntimeUUID::newUUID());
    cv::Mat nativeImage = NSImageToMat(image);
    mPhotobook->lutService()->applyLutInMemory(lutId, lutIndex, nativeImage);
    return [NSString stringWithUTF8String:boost::uuids::to_string(*lutId).c_str()];
}

- (NSString*) getText:(NSString*)key
{
    std::string nativeKey = [key UTF8String];
    auto result = mPhotobook->help(nativeKey);
    return [NSString stringWithUTF8String:result.c_str()];
}

- (void) loadProject:(NSString*)projectId
{
    std::string nativeProjectId = [projectId UTF8String];
    boost::uuids::uuid nativeUuid = boost::uuids::string_generator()(nativeProjectId);
    mPhotobook->projectManagementService()->loadProject(nativeUuid);
}

- (void) saveProject
{
    mPhotobook->projectManagementService()->save();
}

- (void)applyTransformationOnDisk:(NSString*)transformationId lutIndex:(unsigned)lutIndex image:(FrontendImage*)image saturation:(double)saturation contrast:(double)contrast brightness:(double)brightness
{
    std::string uuidStr = [transformationId UTF8String];
    boost::uuids::uuid nativeUuid;
    try {
        
        boost::uuids::string_generator gen;
        nativeUuid = gen(uuidStr);
    }
    catch (const std::exception& e) {
        PBDev::basicAssert(false);
    }
    PBDev::LutApplicationId transformaionId = PBDev::LutApplicationId(nativeUuid);
    
    PB::GenericImagePtr nativeImage = [image unwrap];
    mPhotobook->lutService()->applyTransformationOnDisk(transformaionId, lutIndex, nativeImage, saturation, contrast, brightness);
}

@end
