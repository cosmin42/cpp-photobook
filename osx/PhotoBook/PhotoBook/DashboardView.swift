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
    
    init(buttonBackgroundColor:Color, photobook: Photobook)
    {
        self.buttonBackgroundColor = buttonBackgroundColor
        _photobook = State(initialValue: photobook)
        
        _paperSetting = State(initialValue: PaperSettings.getDefaultSettings(PaperType.A4_Landscape))
        
        _paperWidthText = State(initialValue: String(self.paperSetting.width))
        _paperHeightText = State(initialValue: String(self.paperSetting.height))
        _paperPpiText = State(initialValue: String(self.paperSetting.ppi))
    }
    
    //var photobook: Photobook
    var body: some View {
        HStack {
            Spacer()
            HStack {
                Spacer()
                // Button with specific size in the center
                Button(action: {
                    print("Button was tapped!")
                    isDialogVisible = true
                }) {
                    Text("+").frame(minWidth: 100, minHeight: 100)
                        .padding()
                        .background(buttonBackgroundColor)
                }
                .buttonStyle(PlainButtonStyle())
                .foregroundColor(.white)
                .cornerRadius(8)
                .sheet(isPresented: $isDialogVisible) {
                    // Dialog content
                    NewProjectDialog(isVisible: $isDialogVisible, selectedOption: $selectedOption, paperWidthText: $paperWidthText, paperHeightText: $paperHeightText, paperPpiText:$paperPpiText, paperSettings: $paperSetting, photobook: $photobook, options: options)
                }
                Spacer()
            }
            
            LazyVGrid(columns: columns, spacing: 10) {
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
                DeleteProjectDialog(isDeleteDialogVisible: $isDeleteDialogVisible, projectName: $toRenameProjectName, photobook: $photobook)
            }
            .frame(alignment: .center)
            .padding()
        }
        .padding()
        .onAppear()
        {
            PhotoBookApp.setListener(listener: self)
            self.photobook.start()
            self.photobook.recallMetadata()
        }
    }
    
    func onProjectRead(){
        
    }
    
    func onMetadataUpdated(){
        projectsList = photobook.projectsList()
        
        let columnsCount: Int = Int(sqrt(Double(projectsList.count)))
        
        columns.removeAll()
        
        for _ in 0..<columnsCount {
            columns.append(GridItem(spacing: 10, alignment: .leading))
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
    
    let options: [String]
    
    var body: some View {
        VStack(spacing: 20) {
            Text("New Album")
                .font(.headline)
            
            Picker("ALbum Settings", selection: $selectedOption) {
                ForEach(options, id: \.self) { option in
                    Text(option).tag(option)
                }
            }
            .onChange(of: selectedOption) { newValue in
                if (newValue != "Custom")
                {
                    paperSettings = PaperSettings.getDefaultSettings(byName: newValue)
                    
                    paperWidthText = String(paperSettings.width)
                    paperWidthText = String(paperSettings.height)
                    paperPpiText = String(paperSettings.ppi)
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
                            selectedOption = "Custom"
                        }
                    Text("px")
                }
                HStack {
                    Text("Paper Height:")
                    TextField(String(paperSettings.height), text: $paperHeightText)
                        .textFieldStyle(RoundedBorderTextFieldStyle())
                        .onChange(of: paperHeightText) { newValue in
                            selectedOption = "Custom"
                        }
                    Text("px")
                }
                HStack {
                    Text("Paper Finess:")
                    TextField(String(paperSettings.ppi), text: $paperPpiText)
                        .textFieldStyle(RoundedBorderTextFieldStyle())
                        .onChange(of: paperPpiText) { newValue in
                            selectedOption = "Custom"
                        }
                    Text("ppi")
                }
            }
            HStack {
                Button("Create") {
                    // Close the dialog
                    isVisible = false
                    
                    let projectName = photobook.generateProjectName()
                    photobook.newProject(projectName, paperSettings: paperSettings)
                }
                .background(Color.blue)
                .foregroundColor(.white)
                .cornerRadius(8)
                // TODO: Work out the colors for the buttons
                Button("Cancel") {
                    // Close the dialog
                    isVisible = false
                }
                .background(Color.blue)
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
    var body: some View {
        VStack(spacing: 20) {
            Text("Rename Album")
                .font(.headline)
            HStack {
                Text("Project Name:")
                TextField("Project placeholder name", text: $projectName)
                    .textFieldStyle(RoundedBorderTextFieldStyle())
            }
            
            HStack {
                Button("Rename")
                {
                    isRenameDialogVisible = false
                }
                
                Button("Cancel")
                {
                    isRenameDialogVisible = false
                }
            }
        }
    }
}

struct DeleteProjectDialog: View {
    @Binding var isDeleteDialogVisible: Bool
    @Binding var projectName: String
    @Binding var photobook: Photobook
    var body: some View {
        VStack(spacing: 20) {
            Text("Rename Album")
                .font(.headline)
            HStack {
                Text("Type DELETE:")
                TextField("", text: $projectName)
                    .textFieldStyle(RoundedBorderTextFieldStyle())
            }
            
            HStack {
                Button("Delete")
                {
                    // TODO: Add delete validation
                    isDeleteDialogVisible = false
                }
                
                Button("Cancel")
                {
                    isDeleteDialogVisible = false
                }
            }
        }
    }
}

struct DashboardView_Previews: PreviewProvider {
    static var previews: some View {
        DashboardView(buttonBackgroundColor: Color(red:21.6/100, green:26.3/100, blue:27.5/100), photobook: Photobook())
    }
}
