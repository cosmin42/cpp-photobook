//
//  PhotoBookApp.swift
//  PhotoBook
//
//  Created by Cosmin Mihai on 31.08.2023.
//

import SwiftUI

private var photobookUIListener: PhotobookUIListener? = nil;

@objc extension PhotobookListenerWrapperCLevel
{

    func onProjectRead() {
        photobookUIListener?.onProjectRead()
    }
    
    func onMetadataUpdated()
    {
        photobookUIListener?.onMetadataUpdated()
    }
}


@main
struct PhotoBookApp: App, PhotobookUIListener {
    @State var photobook: Photobook = Photobook()
    @State var photobookListenerWrapperCLevel = PhotobookListenerWrapperCLevel()
    init()
    {
        photobookUIListener = self;
        self.photobook.setPhotobookListener(photobookListenerWrapperCLevel)
        
    }
    
    var body: some Scene {
        WindowGroup {
            DashboardView(buttonBackgroundColor: Color(red:21.6/100, green:26.3/100, blue:27.5/100), photobook:self.photobook)
        }
    }
    
    func onProjectRead(){}
    func onMetadataUpdated(){}
    
    static func setListener(listener: PhotobookUIListener)
    {
        photobookUIListener = listener
    }
}
