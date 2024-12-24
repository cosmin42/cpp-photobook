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
    @State var navigationPath: [String] = []
    @State private var isPropertiesDetailsDialogVisible = false
    init()
    {
        photobookUIListener = self;
        self.photobook.setPhotobookListener(photobookListenerWrapperCLevel)
        self.photobook.setNoirListener(noirListenerWrapperCLevel)
        navigationPath = ["Dashboard"]
    }
    
    var body: some Scene {
        WindowGroup {
            NavigationStack (path: $navigationPath) {
                DashboardView(navigationPath:$navigationPath, buttonBackgroundColor: Color(red:21.6/100, green:26.3/100, blue:27.5/100), photobook:self.photobook)
                    .navigationDestination(for: String.self) { value in
                        if value == "Dashboard" {
                            DashboardView(navigationPath: $navigationPath, buttonBackgroundColor: Color(red:21.6/100, green:26.3/100, blue:27.5/100), photobook:self.photobook)
                        }
                        else if value == "Table" {
                            TableContentView(navigationPath: $navigationPath, photobook:self.photobook)
                        }
                    }
            }
            .sheet(isPresented: $isPropertiesDetailsDialogVisible)
            {
                PropertiesDetailsDialog(isPropertiesDetailsDialogVisible: $isPropertiesDetailsDialogVisible, photobook: $photobook)
            }
        }
        .commands {
            CommandGroup(after: .newItem) {
                if navigationPath.last == "Table" {
                    Button("Properties") {
                        isPropertiesDetailsDialogVisible = true;
                    }
                    .keyboardShortcut("P", modifiers: [.command, .shift]) // Add a shortcut
                }
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

struct PropertiesDetailsDialog: View {
    @Binding var isPropertiesDetailsDialogVisible: Bool
    @Binding var photobook: Photobook
    var body: some View {
        Text("Lorem ipsum dolor")
        Button(action:{isPropertiesDetailsDialogVisible = false;}){
            Text("Close")
        }
    }
}
