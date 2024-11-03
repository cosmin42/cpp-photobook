//
//  PhotoBookApp.swift
//  PhotoBook
//
//  Created by Cosmin Mihai on 31.08.2023.
//

import SwiftUI

@objc extension PhotobookListenerWrapperCLevel
{
    func onProjectRead() {
        NSLog("Test project read.")
    }
    
    func onMetadataUpdated()
    {
        NSLog("Project metadata updated.")
    }
}


@main
struct PhotoBookApp: App {
    @State var photobook: Photobook = Photobook()
    @State var photobookListenerWrapperCLevel = PhotobookListenerWrapperCLevel()
    init()
    {
        self.photobook.setPhotobookListener(photobookListenerWrapperCLevel)
    }
    
    var body: some Scene {
        WindowGroup {
            DashboardView(buttonBackgroundColor: Color(red:21.6/100, green:26.3/100, blue:27.5/100), photobook:self.photobook)
        }
    }
}
