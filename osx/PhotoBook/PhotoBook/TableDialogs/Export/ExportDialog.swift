//
//  ExportDialog.swift
//  PhotoBook
//
//  Created by Cosmin Mihai on 01.03.2025.
//


import SwiftUI

struct ExportDialog: View {
    
    @Binding var isPresented: Bool
    @State private var showNotification: Bool = false
    @ObservedObject var model: ExportModel
    
    var body: some View {
        VStack(spacing: 20) {
            Text("Export Album")
                .font(.headline)
            
            HStack {
                Toggle("", isOn: $model.exportPdf)
                    .frame(width: 80, height: 20)
                Text("PDF")
                Spacer()
            }
            
            HStack {
                Toggle("", isOn: $model.exportPdfOptimized)
                    .frame(width: 80, height: 20)
                Text("PDF (optimized)")
                Spacer()
            }
            
            HStack {
                Toggle("", isOn: $model.exportJpg)
                    .frame(width: 80, height: 20)
                Text("JPG")
                Spacer()
            }
            
            HStack{
                Text("Album name: ")
                    .frame(width: 150)
                TextField("Album name", text: $model.albumName)
                    .frame(width: 200, height: 40)
                    .textFieldStyle(PlainTextFieldStyle())
                    .background(
                        RoundedRectangle(cornerRadius: 8)
                            .fill(Color.TextFieldBackgroundColor) // Background color
                    )
                    .overlay(
                        RoundedRectangle(cornerRadius: 8)
                            .stroke(Color.BorderColor, lineWidth: 1) // Border
                    )
                    .foregroundColor(Color.MainFontColor)
                    .onAppear {
                        model.albumName = "Album"
                    }
                Spacer()
            }
            
            if showNotification
            {
                if model.albumName.isEmpty {
                    Text("Please specify the album name")
                        .foregroundColor(Color.red)
                }
            }
            
            HStack
            {
                Text("Export path: ")
                    .frame(width: 150)
                TextField("Export path", text: $model.exportPath)
                    .frame(width: 200, height: 40)
                    .textFieldStyle(PlainTextFieldStyle())
                    .background(
                        RoundedRectangle(cornerRadius: 8)
                            .fill(Color.TextFieldBackgroundColor)
                    )
                    .overlay(
                        RoundedRectangle(cornerRadius: 8)
                            .stroke(Color.BorderColor, lineWidth: 1)
                    )
                
                Button("Browse") {
                    openFileBrowser()
                }
                .padding(8)
                .padding(.horizontal, 8)
                .background(Color.ConfirmationButtonBackground)
                .cornerRadius(8)
                Spacer()
            }
            
            if showNotification
            {
                if model.exportPath.isEmpty {
                    Text("Please specify where to export the album")
                        .foregroundColor(Color.MainFontColor)
                }
            }
            
            HStack{
                Button("Export") {
                    if !model.albumName.isEmpty && !model.exportPath.isEmpty
                    {
                        model.onExport(model.albumName, model.exportPath, model.exportPdf, model.exportPdfOptimized, model.exportJpg)
                        self.isPresented = false
                    }
                    else
                    {
                        showNotification = true
                    }
                }
                .padding(8)
                .padding(.horizontal, 8)
                .background(Color.ConfirmationButtonBackground)
                .cornerRadius(8)
                .padding()
            }
            
        }
        .frame(maxWidth: .infinity, maxHeight: .infinity)
        .padding(2)
        .background(Image("export") // Name of your image in Assets
            .resizable()
            .scaledToFill()
            .clipped())
    }
    
    private func openFileBrowser() {
#if os(macOS)
        let panel = NSOpenPanel()
        panel.allowsMultipleSelection = false
        panel.canChooseDirectories = true
        panel.canChooseFiles = false
        
        if panel.runModal() == .OK {
            model.exportPath = panel.url?.path ?? ""
        }
#endif
    }
}




