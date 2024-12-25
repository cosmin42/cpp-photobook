//
//  DashboardView.swift
//  PhotoBook
//
//  Created by Cosmin Mihai on 31.08.2023.
//

import SwiftUI

struct DashboardView: View, PhotobookUIListener {
    var buttonBackgroundColor: Color
    @State var photobook: Photobook
    
    @State private var isDialogVisible = false
    @State private var isRenameDialogVisible = false
    @State private var isDeleteDialogVisible = false;
    @State private var paperSetting: PaperSettings
    @State private var selectedOption = "A4 Landscape"
    @State private var paperWidthText: String = ""
    @State private var paperHeightText: String = ""
    @State private var paperPpiText: String = ""
    @State private var projectsList: [ProjectMetadataEntry] = []
    @State private var toRenameProjectName: String = ""
    @State private var toDeleteProjectName: String = ""
    @State private var navigateToTable = false
    @Binding var navigationPath: [String]
    
    let options = ["A3 Portrait",
                   "A3 Landscape",
                   "A4 Portrait",
                   "A4 Landscape",
                   "A5 Portrait",
                   "A5 Landscape",
                   "Square",
                   "Custom"]
    
    @State var columns = [
        GridItem(), // Each column resizes based on available space
        GridItem(),
        GridItem()
    ]
    
    init(navigationPath:Binding<[String]>, buttonBackgroundColor:Color, photobook: Photobook)
    {
        self.buttonBackgroundColor = buttonBackgroundColor
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
                    
                    // Button with specific size in the center
                    Button(action: {
                        print("Button was tapped!")
                        isDialogVisible = true
                    }) {
                        Text("+").frame(minWidth: 100, minHeight: 100)
                            .padding()
                            .background(buttonBackgroundColor)
                            .frame(alignment:.leading)
                    }
                    .buttonStyle(PlainButtonStyle())
                    .foregroundColor(.white)
                    .cornerRadius(8)
                    .sheet(isPresented: $isDialogVisible) {
                        // Dialog content
                        NewProjectDialog(isVisible: $isDialogVisible, selectedOption: $selectedOption, paperWidthText: $paperWidthText, paperHeightText: $paperHeightText, paperPpiText:$paperPpiText, paperSettings: $paperSetting, photobook: $photobook, options: options)
                    }
                }
                .frame(width: geometry.size.width * 0.5)
                
                LazyHGrid(rows: columns, spacing: 10) {
                    ForEach(projectsList, id: \.self) { item in
                        // Each item in the grid
                        Button(action: {
                            print("Button was pressed!")
                        }){
                            Text("\(item.name)")
                                .frame(minWidth: 100, minHeight: 100)
                                .padding()
                                .background(buttonBackgroundColor)
                            
                        }
                        .buttonStyle(PlainButtonStyle())
                        .foregroundColor(.white)
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
                .frame(alignment:.leading)
                .sheet(isPresented: $isRenameDialogVisible) {
                    RenameProjectDialog(isRenameDialogVisible: $isRenameDialogVisible, projectName:$toRenameProjectName, photobook: $photobook)
                }
                .sheet(isPresented: $isDeleteDialogVisible) {
                    DeleteProjectDialog(isDeleteDialogVisible: $isDeleteDialogVisible, projectName: $toDeleteProjectName, photobook: $photobook, projectDeleteText:"")
                }
                .frame(width: geometry.size.width * 0.5, height: geometry.size.height)
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
            columns.append(GridItem(spacing: 10, alignment: .leading))
        }
        
        if (!focusedName.isEmpty)
        {
            navigationPath.append("Table")
        }
    }
}


struct NewProjectDialog: View {
    @Binding var isVisible: Bool
    @Binding var selectedOption: String
    @Binding var paperWidthText: String
    @Binding var paperHeightText: String
    @Binding var paperPpiText: String
    @Binding var paperSettings: PaperSettings
    @Binding var photobook: Photobook
    
    @State var comboChangeFlag0: Bool = false
    @State var comboChangeFlag1: Bool = false
    @State var comboChangeFlag2: Bool = false
    
    let options: [String]

    var body: some View {
        VStack(spacing: 20) {
            Text("New Album")
                .font(.headline)
            
            Picker("Album Settings", selection: $selectedOption) {
                ForEach(options, id: \.self) { option in
                    Text(option).tag(option)
                }
            }
            .onChange(of: selectedOption) { newValue in
                if (newValue != "Custom")
                {
                    paperSettings = PaperSettings.getDefaultSettings(byName: newValue)
                    
                    paperWidthText = String(paperSettings.width)
                    paperHeightText = String(paperSettings.height)
                    paperPpiText = String(paperSettings.ppi)
                    comboChangeFlag0 = true
                    comboChangeFlag1 = true
                    comboChangeFlag2 = true
                }
            }
            .pickerStyle(MenuPickerStyle()) // Combo box style
            
            // Text input fields
            VStack(alignment: .leading, spacing: 10) {
                HStack {
                    Text("Paper Width:")
                    TextField(String(paperSettings.width), text: $paperWidthText)
                        .textFieldStyle(RoundedBorderTextFieldStyle())
                        .onChange(of: paperWidthText) { newValue in
                            if self.comboChangeFlag0
                            {
                                self.comboChangeFlag0 = false
                            }
                            else
                            {
                                selectedOption = "Custom"
                            }
                        }
                    Text("px")
                }
                HStack {
                    Text("Paper Height:")
                    TextField(String(paperSettings.height), text: $paperHeightText)
                        .textFieldStyle(RoundedBorderTextFieldStyle())
                        .onChange(of: paperHeightText) { newValue in
                            if self.comboChangeFlag1
                            {
                                self.comboChangeFlag1 = false
                            }
                            else
                            {
                                selectedOption = "Custom"
                            }
                        }
                    Text("px")
                }
                HStack {
                    Text("Paper Finess:")
                    TextField(String(paperSettings.ppi), text: $paperPpiText)
                        .textFieldStyle(RoundedBorderTextFieldStyle())
                        .onChange(of: paperPpiText) { newValue in
                            if self.comboChangeFlag2
                            {
                                self.comboChangeFlag0 = false
                            }
                            else
                            {
                                selectedOption = "Custom"
                            }
                        }
                    Text("ppi")
                }
            }
            HStack {
                Button("Create") {
                    // Close the dialog
                    isVisible = false
                    
                    photobook.newProject(paperSettings)
                }
                .background(Color.PrimaryColor)
                .foregroundColor(Color.MainFontColor)
                .cornerRadius(8)
                // TODO: Work out the colors for the buttons
                Button("Cancel") {
                    // Close the dialog
                    isVisible = false
                }
                .background(Color.RemoveButtonBackground)
                .foregroundColor(.white)
                .cornerRadius(8)
            }
            
        }
        .padding()
    }
}

struct RenameProjectDialog: View {
    @Binding var isRenameDialogVisible: Bool
    @Binding var projectName: String
    @Binding var photobook: Photobook
    @State var oldProjectName: String = ""
    
    init(isRenameDialogVisible:Binding<Bool>, projectName:Binding<String>, photobook:Binding<Photobook>)
    {
        _isRenameDialogVisible = isRenameDialogVisible
        _projectName = projectName
        _photobook = photobook
    }
    var body: some View {
        VStack(spacing: 20) {
            Text("Rename Album")
                .font(.headline)
                .padding()
            HStack {
                Text("Project Name:")
                TextField("Project placeholder name", text: $projectName)
                    .textFieldStyle(RoundedBorderTextFieldStyle())
                    .onAppear()
                {
                    self.oldProjectName = projectName
                }
            }
            .padding()
            
            HStack {
                Button("Rename")
                {
                    if self.projectName != ""
                    {
                        self.photobook.projectManagementService().rename(self.oldProjectName, newName: self.projectName)
                    }
                    isRenameDialogVisible = false
                }
                .padding()
                
                Button("Cancel")
                {
                    isRenameDialogVisible = false
                }
                .padding()
            }
            .padding()
        }
        .padding()
    }
}

struct DeleteProjectDialog: View {
    @Binding var isDeleteDialogVisible: Bool
    @Binding var projectName: String
    @Binding var photobook: Photobook
    @State var projectDeleteText: String
    var body: some View {
        VStack(spacing: 20) {
            Text("Rename Album")
                .font(.headline)
                .padding()
            HStack {
                Text("Type DELETE:")
                TextField("", text: $projectDeleteText)
                    .textFieldStyle(RoundedBorderTextFieldStyle())
            }
            .padding()
            
            HStack {
                Button("Delete")
                {
                    if projectDeleteText == "DELETE"
                    {
                        self.photobook.projectManagementService().remove(byName: projectName)
                    }
                    // TODO: Add delete validation
                    isDeleteDialogVisible = false
                }
                
                Button("Cancel")
                {
                    isDeleteDialogVisible = false
                }
            }
            .padding()
        }
        .padding()
    }
}
