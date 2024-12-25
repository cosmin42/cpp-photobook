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
    
    init(navigationPath:Binding<[String]>, photobook: Photobook)
    {
        _photobook = State(initialValue: photobook)
        _navigationPath = navigationPath
        _paperSetting = State(initialValue: PaperSettings.getDefaultSettings(PaperType.A4_Landscape))
        _paperWidthText = State(initialValue: String(self.paperSetting.width))
        _paperHeightText = State(initialValue: String(self.paperSetting.height))
        _paperPpiText = State(initialValue: String(self.paperSetting.ppi))
    }
    
    //var photobook: Photobook
    var body: some View {
        GeometryReader { geometry in
            HStack {
                HStack {
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
                        NewProjectDialog(isVisible: $isNewProjectDialogVisible, selectedOption: $selectedOption, paperWidthText: $paperWidthText, paperHeightText: $paperHeightText, paperPpiText:$paperPpiText, paperSettings: $paperSetting, photobook: $photobook, options: options)
                    }
                }
                .frame(width: geometry.size.width * 0.5)
                ScrollView(.horizontal) {
                    LazyHGrid(rows: columns, alignment: .center, spacing: 10) {
                        ForEach(projectsList, id: \.self) { item in
                            // Each item in the grid
                            Button(action: {
                                print("Button was pressed!")
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
}
