//
//  PhotoBookApp.swift
//  PhotoBook
//
//  Created by Cosmin Mihai on 31.08.2023.
//

import SwiftUI
import Bugsnag

private var photobookUIListener: [PhotobookUIListener] = [];
private var noirUIListener: [NoirUIListener] = []

@objc extension PhotobookListenerWrapperCLevel
{
    func onProjectRead() {
        photobookUIListener.last?.onProjectRead()
    }
    
    func onMetadataUpdated(_ focusedName: String)
    {
        photobookUIListener.last?.onMetadataUpdated(focusedName:focusedName)
    }
    
    func onMappingFinished(_ root: String)
    {
        photobookUIListener.last?.onMappingFinished(root: root)
    }
    
    func onImageUpdated(_ root: String, row:UInt, index:UInt)
    {
        photobookUIListener.last?.onImageUpdated(root: root, row: row, index: index)
    }
    
    func onCollageThumbnailsCreated()
    {
        photobookUIListener.last?.onCollageThumbnailsCreated()
    }
    
    func onImageMapped(_ imageId: String, image: FrontendImage)
    {
        photobookUIListener.last?.onImageMapped(imageId:imageId, image: image)
    }
}

@objc extension NoirListenerWrapperCLevel
{
    func onNoirLutAdded(_ item:LutItem){
        noirUIListener.last?.onNoirLutAdded(item:item)
    }
    func onNoirError(){}
}

@main
struct PhotoBookApp: App, PhotobookUIListener, NoirUIListener {
    @State var photobook: Photobook = Photobook()
    @State var photobookListenerWrapperCLevel = PhotobookListenerWrapperCLevel()
    @State var noirListenerWrapperCLevel = NoirListenerWrapperCLevel()
    @State var navigationPath: [String] = []
    @State private var isPropertiesDetailsDialogVisible = false
    @State private var lutGridModel: LutGridModel = LutGridModel()
    
    init()
    {
        photobookUIListener = [self]
        noirUIListener = [self]
        self.photobook.setPhotobookListener(photobookListenerWrapperCLevel)
        self.photobook.setNoirListener(noirListenerWrapperCLevel)
        navigationPath = ["Dashboard"]
        
        if let apiKey = Bundle.main.object(forInfoDictionaryKey: "BugsnagApiKey") as? String {
            let config = BugsnagConfiguration.loadConfig()
            config.appVersion = "0.0.0-alpha"
            config.apiKey = apiKey
            Bugsnag.start(with: config).notifyError(NSError(domain:"com.example", code:408, userInfo:nil))
        } else {
            print("Bugsnag API Key NOT found in Info.plist")
        }
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
                            TableContentView(navigationPath: $navigationPath, lutGridModel:$lutGridModel, photobook:self.photobook)
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
    func onMappingFinished(root: String) {}
    func onImageUpdated(root: String, row:UInt, index:UInt){}
    func onCollageThumbnailsCreated(){}
    func onImageMapped(imageId: String, image: FrontendImage){}
    func onNoirLutAdded(item:LutItem) {
        lutGridModel.images.append(item)
    }
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
