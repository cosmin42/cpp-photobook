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
    func onImageUpdated(root: String, row:UInt, index:UInt)
    func onCollageThumbnailsCreated()
    func onImageMapped(imageId: String, image: FrontendImage)
    func onCollageCreated(image: FrontendImage)
    func onLutAppliedInMemory(imageId: String, image: NSImage)
}
