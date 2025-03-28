//
//  PhotobookUIListener.swift
//  PhotoBook
//
//  Created by Cosmin Mihai on 03.11.2024.
//

protocol PhotobookUIListener
{
    func onProjectRead()
    func onMetadataUpdated(focusedName: String)
    func onMappingFinished(root: String, imagesCount:UInt32)
    func onSearchingFinished(importedFolderPath: String, placeholders: [String:FrontendImage])
    func onImageUpdated(root: String, row:UInt, index:UInt)
    func onCollageThumbnailsCreated()
    func onImageMapped(imageId: String, image: FrontendImage)
    func onImageCopied(imageId: String, image: FrontendImage)
    func onCollageCreated(image: FrontendImage)
    func onLutAppliedInMemory(imageId: String, image: NSImage)
    func onLutAppliedOnDiskInplace(imageId: String)
    func onLutAppliedOnDisk(imageId: String, image: FrontendImage)
    func onEffectsApplied(imageId: String, image: FrontendImage)
    func onEffectsAppliedInplace(imageId: String)
    func onError(message: String)
}
