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
    
    func onMappingFinished(_ root: String, imagesCount:UInt32)
    {
        photobookUIListener.last?.onMappingFinished(root: root, imagesCount: imagesCount)
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
    
    func onImageCopied(_ imageId: String, image: FrontendImage)
    {
        photobookUIListener.last?.onImageCopied(imageId:imageId, image:image)
    }
    
    func onCollageCreated(_ image: FrontendImage)
    {
        photobookUIListener.last?.onCollageCreated(image: image)
    }
    
    func onLutAppliedInMemory(_ imageId: String, image: NSImage)
    {
        photobookUIListener.last?.onLutAppliedInMemory(imageId:imageId, image:image)
    }
    
    func onLutAppliedOnDiskInplace(_ imageId: String)
    {
        photobookUIListener.last?.onLutAppliedOnDiskInplace(imageId:imageId)
    }
    
    func onLutAppliedOnDisk(_ imageId: String, image: FrontendImage)
    {
        photobookUIListener.last?.onLutAppliedOnDisk(imageId:imageId, image:image)
    }
    
    func onEffectsApplied(_ imageId: String, image: FrontendImage)
    {
        photobookUIListener.last?.onEffectsApplied(imageId:imageId, image:image)
    }
    
    func onEffectsAppliedInplace(_ imageId: String)
    {
        photobookUIListener.last?.onEffectsApplied(imageId:imageId, image:FrontendImage())
    }
    
    func onError(_ message: String)
    {
        photobookUIListener.last?.onError(message: message)
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
    @State private var showLicenseDialog = false
    @State private var aboutDialogVisible = false
    @State private var lutGridModel: LutGridModel = LutGridModel()
    @State private var licenseModel: LicenseModel = LicenseModel()
    @State private var toOpenProjectId: String = ""
    
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
        
        licenseModel.text = self.photobook.getText("license");
    }
    
    var body: some Scene {
        WindowGroup {
            NavigationStack (path: $navigationPath) {
                DashboardView(navigationPath:$navigationPath, toOpenProjectId: $toOpenProjectId, photobook:self.photobook)
                    .navigationDestination(for: String.self) { value in
                        if value == "Dashboard" {
                            DashboardView(navigationPath: $navigationPath, toOpenProjectId: $toOpenProjectId, photobook:self.photobook)
                        }
                        else if value == "Table" {
                            TableContentView(navigationPath: $navigationPath, toOpenProjectId: $toOpenProjectId,  lutGridModel:$lutGridModel, photobook:self.photobook)
                        }
                    }
            }
            .sheet(isPresented: $isPropertiesDetailsDialogVisible)
            {
                PropertiesDetailsDialog(isPropertiesDetailsDialogVisible: $isPropertiesDetailsDialogVisible, photobook: $photobook)
            }
            .sheet(isPresented: $showLicenseDialog)
            {
                LicenseDialog(isPresented: $showLicenseDialog, model: licenseModel)
            }
            .sheet(isPresented: $aboutDialogVisible)
            {
                AboutDialog(isPresented: $aboutDialogVisible)
            }
        }
        .commands {
            CommandGroup(after: .newItem) {
                if navigationPath.last == "Table" {
                    Button("Properties") {
                        isPropertiesDetailsDialogVisible = true;
                    }
                    .keyboardShortcut("P", modifiers: [.command, .shift])
                }
            }
            CommandGroup(replacing: CommandGroupPlacement.help) {
                Button("About") {
                    aboutDialogVisible = true
                }
                Button("License") {
                    showLicenseDialog = true
                }
            }
        }
    }
    
    // TODO: We don't need PhotobookUIListener here
    func onProjectRead() {}
    func onMetadataUpdated(focusedName: String) {}
    func onMappingFinished(root: String, imagesCount: UInt32) {}
    func onImageUpdated(root: String, row:UInt, index:UInt){}
    func onCollageThumbnailsCreated(){}
    func onCollageCreated(image: FrontendImage){}
    func onImageMapped(imageId: String, image: FrontendImage){}
    func onImageCopied(imageId: String, image: FrontendImage) {}

    
    func onNoirLutAdded(item:LutItem) {
        lutGridModel.images.append(item)
    }
    
    func onLutAppliedInMemory(imageId: String, image: NSImage)
    {
        
    }
    
    func onLutAppliedOnDiskInplace(imageId: String)
    {
    }
    
    func onLutAppliedOnDisk(imageId: String, image: FrontendImage)
    {
    }
    
    func onEffectsApplied(imageId: String, image: FrontendImage)
    {
    }
    
    func onEffectsAppliedInplace(imageId: String)
    {
    }
    
    func onError(message: String)
    {
        print("Error: \(message)")
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
