//
//  NewProjectDialogIpad.swift
//  PhotoBook
//
//  Created by Cosmin Mihai on 05.05.2025.
//

import SwiftUI


struct NewProjectDialogIpad: View {
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
    
    private let LABEL_MAX_WIDTH: CGFloat = 200
    private let FIELD_MAX_WIDTH: CGFloat = 200
    private let FIELD_HEIGHT: CGFloat = 60
    
    let options: [String]
    
    var body: some View {
        VStack {
            HStack{
                Spacer()
                Text("New Album")
                    .font(.title)
                Spacer()
            }
            .padding(16)
            Divider()
                .background(.white)
                .padding(.horizontal, 100)
            
            VStack(alignment: .leading, spacing: 20) {
                HStack {
                    Text("Predefined Papers:")
                        .frame(width: LABEL_MAX_WIDTH, alignment: .trailing)
                    
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
                    .frame(width: FIELD_MAX_WIDTH, height: FIELD_HEIGHT, alignment: .leading)
                    .pickerStyle(.menu)
                    .tint(Color.MainFontColor)
                    Spacer()
                }
                HStack {
                    Text("Paper Width:")
                        .frame(width: LABEL_MAX_WIDTH, alignment: .trailing)
                    TextField(String(paperSettings.width), text: $paperWidthText)
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
                        .frame(width: FIELD_MAX_WIDTH, height: FIELD_HEIGHT)
                        .cornerRadius(8)
                        .textFieldStyle(PlainTextFieldStyle())
                        .background(Color.TextFieldBackgroundColor)
                    Text("px")
                    Spacer()
                }
                
                HStack {
                    Text("Paper Height:")
                        .frame(width: LABEL_MAX_WIDTH, alignment: .trailing)
                    TextField(String(paperSettings.height), text: $paperHeightText)
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
                        .frame(width: FIELD_MAX_WIDTH, height: FIELD_HEIGHT)
                        .cornerRadius(8)
                        .textFieldStyle(PlainTextFieldStyle())
                        .background(Color.TextFieldBackgroundColor)
                    Text("px")
                    Spacer()
                }
                
                HStack {
                    Text("Paper Finess:")
                        .frame(width: LABEL_MAX_WIDTH, alignment: .trailing)
                    TextField(String(paperSettings.ppi), text: $paperPpiText)
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
                        .frame(width: FIELD_MAX_WIDTH, height: FIELD_HEIGHT)
                        .cornerRadius(8)
                        .textFieldStyle(PlainTextFieldStyle())
                        .background(Color.TextFieldBackgroundColor)
                    Text("ppi")
                }
            }
            Spacer()
            HStack {
                Spacer()
                Spacer()
                Button(action: {
                    // Close the dialog
                    isVisible = false
                    
                    photobook.newProject(paperSettings)
                }) {
                    Text("Create")
                }
                .padding(16)
                .padding(.horizontal, 8)
                .background(Color.PrimaryColorIPad)
                .cornerRadius(8)
                Spacer()
                
                Button("Cancel") {
                    isVisible = false
                }
                .padding(16)
                .padding(.horizontal, 8)
                .background(Color.RemoveButtonBackgroundIPad)
                .cornerRadius(8)
                Spacer()
                Spacer()
            }
            Spacer()
            Spacer()
            Spacer()
        }
        .frame(maxWidth: .infinity, maxHeight: .infinity)
        .padding(2)
        .background(Color.PrimaryColor)
    }
}

