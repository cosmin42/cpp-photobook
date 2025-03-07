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
        
        VStack {
            Text("Export Album")
                .font(.title)
            
            VStack(alignment: .leading) {
                
                HStack {
                    Toggle("", isOn: $model.exportPdf)
                    Text("PDF")
                }
                
                HStack {
                    Toggle("", isOn: $model.exportPdfOptimized)
                    Text("PDF (optimized)")
                }
                
                HStack {
                    Toggle("", isOn: $model.exportJpg)
                    Text("JPG")
                }
                Spacer().padding(1)

                TextField("Album name", text: $model.albumName)
                    .onAppear {
                        model.albumName = "Album"
                    }
                    .frame(width: 200)
                
                if showNotification
                {
                    if model.albumName.isEmpty {
                        Text("Please specify the album name")
                            .foregroundColor(Color.red)
                    }
                }
                
                HStack
                {
                    TextField("Export path", text: $model.exportPath)
                        .frame(width: 200)
                    
                    Button("Browse") {
                        openFileBrowser()
                    }
                }
                
                if showNotification
                {
                    if model.exportPath.isEmpty {
                        Text("Please specify where to export the album")
                            .foregroundColor(Color.red)
                    }
                }
            }
            
            Spacer().padding(1)
            
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
                .foregroundColor(Color.MainFontColor)
                .cornerRadius(8)
                
                Button("Close") {
                    self.isPresented = false
                }
                .background(Color.RemoveButtonBackground)
                .foregroundColor(Color.MainFontColor)
                .cornerRadius(8)
            }
            
        }
        .padding()
    }
    
    private func openFileBrowser() {
        let panel = NSOpenPanel()
        panel.allowsMultipleSelection = false
        panel.canChooseDirectories = true
        panel.canChooseFiles = false
        
        if panel.runModal() == .OK {
            model.exportPath = panel.url?.path ?? ""
        }
    }
}




