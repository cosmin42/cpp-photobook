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
    private let FIELD_HEIGHT: CGFloat = 40
    
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
                        .textFieldStyle(PlainTextFieldStyle())
                        .background(
                            RoundedRectangle(cornerRadius: 8)
                                .fill(Color.TextFieldBackgroundColor) // Background color
                        )
                        .overlay(
                            RoundedRectangle(cornerRadius: 8)
                                .stroke(Color.BorderColor, lineWidth: 1) // Border
                        )
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
                        .textFieldStyle(PlainTextFieldStyle())
                        .background(
                            RoundedRectangle(cornerRadius: 8)
                                .fill(Color.TextFieldBackgroundColor) // Background color
                        )
                        .overlay(
                            RoundedRectangle(cornerRadius: 8)
                                .stroke(Color.BorderColor, lineWidth: 1) // Border
                        )
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
                        .textFieldStyle(PlainTextFieldStyle())
                        .background(
                            RoundedRectangle(cornerRadius: 8)
                                .fill(Color.TextFieldBackgroundColor) // Background color
                        )
                        .overlay(
                            RoundedRectangle(cornerRadius: 8)
                                .stroke(Color.BorderColor, lineWidth: 1) // Border
                        )
                        
                    Text("ppi")
                }
            }
            HStack {
                Spacer()
                Button(action: {
                    // Close the dialog
                    isVisible = false
                    
                    photobook.newProject(paperSettings)
                }) {
                    Text("Create")
                }
                .padding(8)
                .padding(.horizontal, 8)
                .background(Color.PrimaryColorIPad)
                .cornerRadius(8)
                .padding()
                Spacer()
            }
            Spacer()
            Spacer()
            Spacer()
        }
        .frame(maxWidth: .infinity, maxHeight: .infinity)
        .padding(2)
        .background(Image("new_project") // Name of your image in Assets
            .resizable()
            .scaledToFill()
            .clipped())
    }
}

