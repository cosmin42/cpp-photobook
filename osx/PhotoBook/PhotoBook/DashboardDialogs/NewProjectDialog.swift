//
//  NewProjectDialog.swift
//  PhotoBook
//
//  Created by Cosmin Mihai on 26.12.2024.
//

import SwiftUI


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
                    // TODO: Add validation to the width/height etc
                    paperWidthText = String(paperSettings.width)
                    paperHeightText = String(paperSettings.height)
                    paperPpiText = String(paperSettings.ppi)
                    comboChangeFlag0 = true
                    comboChangeFlag1 = true
                    comboChangeFlag2 = true
                }
            }
            .pickerStyle(MenuPickerStyle())
            
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
                Button("Cancel") {
                    isVisible = false
                }
                .background(Color.RemoveButtonBackground)
                .foregroundColor(Color.MainFontColor)
                .cornerRadius(8)
            }
            
        }
        .padding()
    }
}

