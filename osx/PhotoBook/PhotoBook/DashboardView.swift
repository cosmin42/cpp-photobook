//
//  DashboardView.swift
//  PhotoBook
//
//  Created by Cosmin Mihai on 31.08.2023.
//

import SwiftUI

struct DashboardView: View {
    var buttonBackgroundColor: Color
    var photobook: Photobook
    
    @State private var isDialogVisible = false
    @State private var selectedOption = "A4 Landscape"
    @State private var textField1 = ""
    @State private var textField2 = ""
    @State private var paperSetting: PaperSettings = PaperSettings.getDefaultSettings(PaperType)
    
    let options = ["A3 Portrait",
                   "A3 Landscape",
                   "A4 Portrait",
                   "A4 Landscape",
                   "A5 Portrait",
                   "A5 Landscape",
                   "Square",
                   "Custom"]
    
    init(buttonBackgroundColor:Color, photobook: Photobook)
    {
        self.buttonBackgroundColor = buttonBackgroundColor
        self.photobook = photobook
    }
    
    //var photobook: Photobook
    var body: some View {
        VStack {
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
                    DialogView(isVisible: $isDialogVisible, selectedOption: $selectedOption, textField1: $textField1, textField2: $textField2, paperSettings: $paperSetting, options: options)
                }
                Spacer()
            }
            Spacer()
        }
        .padding()
        .onAppear()
        {
            self.photobook.start()
        }
    }
}


struct DialogView: View {
    @Binding var isVisible: Bool
    @Binding var selectedOption: String
    @Binding var textField1: String
    @Binding var textField2: String
    @Binding var paperSettings: PaperSettings
    
    let options: [String]
    
    var body: some View {
        VStack(spacing: 20) {
            Text("Select an Option")
                .font(.headline)
            
            Picker("Select an Option", selection: $selectedOption) {
                ForEach(options, id: \.self) { option in
                    Text(option).tag(option)
                }
            }
            .pickerStyle(MenuPickerStyle()) // Combo box style
            
            // Text input fields
            VStack(alignment: .leading, spacing: 10) {
                HStack {
                    Text("Paper Width:")
                    TextField("", text: $textField1)
                        .textFieldStyle(RoundedBorderTextFieldStyle())
                        .onAppear()
                    {
                        
                    }
                    Text("px")
                }
                HStack {
                    Text("Paper Height:")
                    TextField("", text: $textField2)
                        .textFieldStyle(RoundedBorderTextFieldStyle())
                    Text("px")
                }
                HStack {
                    Text("Paper Finess:")
                    TextField("", text: $textField2)
                        .textFieldStyle(RoundedBorderTextFieldStyle())
                    Text("ppi")
                }
            }
            
            Button("Done") {
                // Close the dialog
                isVisible = false
            }
            .padding()
            .background(Color.blue)
            .foregroundColor(.white)
            .cornerRadius(8)
        }
        .padding()
    }
}




struct DashboardView_Previews: PreviewProvider {
    static var previews: some View {
        DashboardView(buttonBackgroundColor: Color(red:21.6/100, green:26.3/100, blue:27.5/100), photobook: Photobook())
    }
}
