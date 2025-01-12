//
//  PhotoBookApp.swift
//  PhotoBook
//
//  Created by Cosmin Mihai on 31.08.2023.
//

import SwiftUI

private var photobookUIListener: [PhotobookUIListener] = [];

@objc extension PhotobookListenerWrapperCLevel
{
    func onProjectRead() {
        photobookUIListener.last?.onProjectRead()
    }
    
    func onMetadataUpdated(_ focusedName: String)
    {
        photobookUIListener.last?.onMetadataUpdated(focusedName:focusedName)
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
        photobookUIListener = [self];
        self.photobook.setPhotobookListener(photobookListenerWrapperCLevel)
        self.photobook.setNoirListener(noirListenerWrapperCLevel)
        navigationPath = ["Dashboard"]
    }
    
    var body: some Scene {
        WindowGroup {
            NavigationStack (path: $navigationPath) {
                DashboardView(navigationPath:$navigationPath, photobook:self.photobook)
                    .navigationDestination(for: String.self) { value in
                        if value == "Dashboard" {
                            DashboardView(navigationPath: $navigationPath, photobook:self.photobook)
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
            CommandGroup(replacing: CommandGroupPlacement.help) {
                Button("App Help") {
                }
                Divider()
                Button("License") {
                }
            }
            
        }
    }
    
    // TODO: We don't need PhotobookUIListener here
    func onProjectRead() {}
    func onMetadataUpdated(focusedName: String) {}
    func onNoirLutAdded() {}
    func onNoirError() {}
    
    static func pushListener(listener: PhotobookUIListener)
    {
        photobookUIListener.append(listener)
    }
    
    static func popListener()
    {
        photobookUIListener.removeLast()
    }
}

struct PropertiesDetailsDialog: View {
    @Binding var isPropertiesDetailsDialogVisible: Bool
    @Binding var photobook: Photobook
    @State var details = "Lorem ipsum dolor"
    
    var body: some View {
        Text(details)
            .onAppear()
        {
            var paperSettings = self.photobook.projectManagementService().paperSettings()
            self.details = """
            Paper Type: \(paperSettings?.paperType)
            Paper Width: \(paperSettings?.width)
            Paper Height: \(paperSettings?.height)
            """
        }
        .padding()
        Button(action:{isPropertiesDetailsDialogVisible = false;}){
            Text("Close")
        }
        .padding()
    }
}
