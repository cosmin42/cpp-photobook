//
//  DashboardView.swift
//  PhotoBook
//
//  Created by Cosmin Mihai on 31.08.2023.
//

import SwiftUI

struct DashboardView: View, PhotobookUIListener {
    @State var photobook: Photobook
    
    @State private var isNewProjectDialogVisible = false
    @State private var isRenameDialogVisible = false
    @State private var isDeleteDialogVisible = false
    
    @State private var paperSetting: PaperSettings
    @State private var selectedOption = "A4 Landscape"
    @State private var paperWidthText: String = ""
    @State private var paperHeightText: String = ""
    @State private var paperPpiText: String = ""
    
    @State private var projectsList: [ProjectMetadataEntry] = []
    
    @State private var toRenameProjectName: String = ""
    @State private var toDeleteProjectName: String = ""
    
    @Binding var navigationPath: [String]
    
    let options = ["A3 Portrait",
                   "A3 Landscape",
                   "A4 Portrait",
                   "A4 Landscape",
                   "A5 Portrait",
                   "A5 Landscape",
                   "Square",
                   "Custom"]
    
    @State var columns:[GridItem] = []
    
    @Binding private var toOpenProjectId: String
    
    init(navigationPath:Binding<[String]>, toOpenProjectId:Binding<String>, photobook: Photobook)
    {
        _toOpenProjectId = toOpenProjectId
        _photobook = State(initialValue: photobook)
        _navigationPath = navigationPath
        _paperSetting = State(initialValue: PaperSettings.getDefaultSettings(PaperType.A4_Landscape))
        _paperWidthText = State(initialValue: String(self.paperSetting.width))
        _paperHeightText = State(initialValue: String(self.paperSetting.height))
        _paperPpiText = State(initialValue: String(self.paperSetting.ppi))
    }
    
    var body: some View {
        GeometryReader { geometry in
            HStack {
                ZStack {
                    Button(action: {
                        isNewProjectDialogVisible = true
                    }) {
                        Text("+").frame(minWidth: 100, minHeight: 100)
                            .padding()
                            .background(Color.ButtonBackgroundColor)
                            .frame(alignment:.leading)
                    }
                    .buttonStyle(PlainButtonStyle())
                    .foregroundColor(.white)
                    .cornerRadius(8)
                    .sheet(isPresented: $isNewProjectDialogVisible) {
                        // Dialog content
#if os(macOS)
                        NewProjectDialog(isVisible: $isNewProjectDialogVisible, selectedOption: $selectedOption, paperWidthText: $paperWidthText, paperHeightText: $paperHeightText, paperPpiText:$paperPpiText, paperSettings: $paperSetting, photobook: $photobook, options: options)
#else
                        NewProjectDialogIpad(isVisible: $isNewProjectDialogVisible, selectedOption: $selectedOption, paperWidthText: $paperWidthText, paperHeightText: $paperHeightText, paperPpiText:$paperPpiText, paperSettings: $paperSetting, photobook: $photobook, options: options)
#endif
                    }
                }
                .frame(width: geometry.size.width * NoirConstants.GoldenRatioPercentHead)
                ScrollView(.horizontal) {
                    LazyHGrid(rows: columns, alignment: .center, spacing: 10) {
                        ForEach(projectsList, id: \.self) { item in
                            // Each item in the grid
                            Button(action: {
                                toOpenProjectId = item.identifier
                                navigationPath.append("Table")
                            }){
                                Text("\(item.name)")
                                    .frame(width: 100, height: 100)
                                    .background(Color.ButtonBackgroundColor)
                            }
                            .frame(width: 100, height: 100, alignment:.center)
                            .buttonStyle(PlainButtonStyle())
                            .foregroundColor(Color.MainFontColor)
                            .cornerRadius(8)
                            .contextMenu {
                                Button(action: {
                                    toRenameProjectName = item.name
                                    isRenameDialogVisible = true
                                }) {
                                    Text("Rename")
                                    Image(systemName: "pencil")
                                }
                                Button(action: {
                                    toDeleteProjectName = item.name
                                    isDeleteDialogVisible = true
                                    print("Pressed delete")
                                }) {
                                    Text("Delete")
                                    Image(systemName: "trash")
                                }
                            }
                        }
                    }
                    .sheet(isPresented: $isRenameDialogVisible) {
                        RenameProjectDialog(isRenameDialogVisible: $isRenameDialogVisible, projectName:$toRenameProjectName, photobook: $photobook)
                    }
                    .sheet(isPresented: $isDeleteDialogVisible) {
                        DeleteProjectDialog(isDeleteDialogVisible: $isDeleteDialogVisible, projectName: $toDeleteProjectName, photobook: $photobook, projectDeleteText:"")
                    }
                    .frame(alignment:.leading)
                }
            }
            .padding()
            .onAppear()
            {
                PhotoBookApp.pushListener(listener: self)
                self.photobook.start()
                self.photobook.recallMetadata()
            }
            .foregroundColor(Color.MainFontColor)
            .background(Color.PrimaryColor)
        }
    }
    
    func onProjectRead(){
        
    }
    
    func onMetadataUpdated(focusedName: String){
        projectsList = photobook.projectsList()
        
        let columnsCount: Int = Int(sqrt(Double(projectsList.count))) + 1
        
        columns.removeAll()
        
        for _ in 0..<columnsCount {
            columns.append(GridItem(.fixed(100)))
        }
        
        if (!focusedName.isEmpty)
        {
            navigationPath.append("Table")
        }
    }
    
    func onMappingFinished(root: String, imagesCount:UInt32)
    {
        
    }
    
    func onSearchingFinished(importedFolderPath: String, placeholders: [String:FrontendImage])
    {
    }
    
    
    func onImageUpdated(root: String, row:UInt, index:UInt){}
    
    func onCollageThumbnailsCreated(){}
    
    func onImageMapped(imageId: String, image: FrontendImage){}
    
    func onImageCopied(imageId: String, image: FrontendImage){}
    
    func onCollageCreated(image: FrontendImage){}
#if os(macOS)
    func onLutAppliedInMemory(imageId: String, image: NSImage) {}
#else
    func onLutAppliedInMemory(imageId: String, image: UIImage) {}
#endif
    
    func onLutAppliedOnDiskInplace(imageId: String) {}
    
    func onLutAppliedOnDisk(imageId: String, image: FrontendImage){}
    
    func onEffectsApplied(imageId: String, image: FrontendImage){}
    
    func onEffectsAppliedInplace(imageId: String){}
    
    func onError(message: String){}
}
