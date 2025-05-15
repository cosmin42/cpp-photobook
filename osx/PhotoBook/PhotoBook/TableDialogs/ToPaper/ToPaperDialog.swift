//
//  ToPaperDialog.swift
//  PhotoBook
//
//  Created by Cosmin Mihai on 23.02.2025.
//

// A view that contains an ok and a cancel button

import SwiftUI

struct ToPaperDialog: View {
    @ObservedObject var model: ToPaperModel
    
    @State private var options = ["Fill", "Fit"]
    
    let columns = [
        GridItem(.flexible()),
        GridItem(.flexible()),
        GridItem(.flexible())
    ]
    
    var body: some View {
        VStack {
            Text("To Photobook Paper")
                .font(.headline)
            
            ScrollView {
                LazyVGrid(columns: columns, spacing: 10) {
                    Text("Image")
                    Text("Resize type")
                    Text("Background Color")
#if os(macOS)
                    ForEach(Array(model.images.keys), id: \.self) { key in
                        if let currentImage = model.images[key],
                           let fileName = currentImage.image.resources().small,
                           let nsImage = NSImage(contentsOfFile: fileName)
                        {
                            
                            Image(nsImage: nsImage)
                                .cornerRadius(10)
                                .frame(height: 80)
                                .padding(4)
                            
                            VStack {
                                let selection = Binding<String>(
                                    
                                    get: {
                                        return currentImage.resizeType
                                    },
                                    
                                    set: { newValue in
                                        currentImage.resizeType = newValue
                                        model.images = model.images
                                    }
                                )
                                
                                Picker("", selection: selection) {
                                    ForEach(options, id: \.self) { option in
                                        Text(option).tag(option)
                                    }
                                }
                                .pickerStyle(SegmentedPickerStyle())
                            }
                            
                            ColorPicker("", selection: Binding<Color>(
                                get: {
                                    return currentImage.backgroundColor
                                },
                                set: { newValue in
                                    currentImage.backgroundColor = newValue
                                    model.images = model.images
                                }
                            ))
                        }
                    }
#else
                    ForEach(Array(model.images.keys), id: \.self) { key in
                        if let currentImage = model.images[key],
                           let fileName = currentImage.image.resources().small,
                           let uiImage = UIImage(contentsOfFile: fileName)
                        {
                            
                            Image(uiImage: uiImage)
                                .cornerRadius(10)
                                .frame(height: 80)
                                .padding(4)
                            
                            VStack {
                                let selection = Binding<String>(
                                    
                                    get: {
                                        return currentImage.resizeType
                                    },
                                    
                                    set: { newValue in
                                        currentImage.resizeType = newValue
                                        model.images = model.images
                                    }
                                )
                                
                                Picker("", selection: selection) {
                                    ForEach(options, id: \.self) { option in
                                        Text(option).tag(option)
                                    }
                                }
                                .pickerStyle(SegmentedPickerStyle())
                            }
                            
                            ColorPicker("", selection: Binding<Color>(
                                get: {
                                    return currentImage.backgroundColor
                                },
                                set: { newValue in
                                    currentImage.backgroundColor = newValue
                                    model.images = model.images
                                }
                            ))
                        }
                    }
#endif
                }
            }
            HStack {
                Text("Set for all")
                    .frame(alignment: .leading)
                Button(action: {
                    for key in model.images.keys {
                        model.images[key]!.resizeType = "Fit"
                        model.images = model.images
                    }
                }) {
                    Text("Fit")
                        .foregroundColor(Color.MainFontColor)
                }
                .padding()
                .background(Color.NeutralButtonBackgroundColor)
                .frame(alignment: .leading)
                
                Button(action: {
                    for key in model.images.keys {
                        model.images[key]!.resizeType = "Fill"
                        model.images = model.images
                    }
                }) {
                    Text("Fill")
                        .foregroundColor(Color.MainFontColor)
                }
                .padding()
                .background(Color.NeutralButtonBackgroundColor)
                .frame(alignment: .leading)
            }
            .frame(maxWidth: .infinity, alignment: .leading)
            
            HStack {
                Button(action: {
                    self.model.onOk()
                    self.model.showDialog.toggle()
                }) {
                    Text("Ok")
                }
                .padding()
                .background(Color.ConfirmationButtonBackground)
                .foregroundColor(Color.MainFontColor)
                .cornerRadius(8)
                
                Button(action: {
                    self.model.images.removeAll()
                    self.model.showDialog.toggle()
                }) {
                    Text("Cancel")
                }
                .padding()
                .background(Color.RemoveButtonBackground)
                .foregroundColor(Color.MainFontColor)
                .cornerRadius(8)
            }
        }
        .padding(12)
        .background(Color.PrimaryBackgroundColor)
    }
}
