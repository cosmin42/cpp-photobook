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
    
    func onMetadataUpdated(_ focusedName: String)
    {
        photobookUIListener?.onMetadataUpdated(focusedName:focusedName)
    }
}

@objc extension NoirListenerWrapperCLevel
{
    func onNoirLutAdded(){}
    func onNoirError(){}
}


@main
struct PhotoBookApp: App, PhotobookUIListener, NoirUIListener {
    @State var photobook: Photobook = Photobook()
    @State var photobookListenerWrapperCLevel = PhotobookListenerWrapperCLevel()
    @State var noirListenerWrapperCLevel = NoirListenerWrapperCLevel()
    init()
    {
        photobookUIListener = self;
        self.photobook.setPhotobookListener(photobookListenerWrapperCLevel)
        self.photobook.setNoirListener(noirListenerWrapperCLevel)
        
    }
    
    var body: some Scene {
        WindowGroup {
            NavigationStack {
                DashboardView(buttonBackgroundColor: Color(red:21.6/100, green:26.3/100, blue:27.5/100), photobook:self.photobook)
            }
        }
    }
    
    // TODO: We don't need PhotobookUIListener here
    func onProjectRead() {}
    func onMetadataUpdated(focusedName: String) {}
    func onNoirLutAdded() {}
    func onNoirError() {}
    
    static func setListener(listener: PhotobookUIListener)
    {
        photobookUIListener = listener
    }
}
