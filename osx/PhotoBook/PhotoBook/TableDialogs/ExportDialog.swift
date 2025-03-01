//
//  ExportDialog.swift
//  PhotoBook
//
//  Created by Cosmin Mihai on 01.03.2025.
//


import SwiftUI

struct ExportDialog: View {
    
    @Binding var isPresented: Bool
    @State private var exportPath: String = ""
    
    var body: some View {
        
        VStack {
            Text("Export Album")
                .font(.title)
            
            VStack(alignment: .leading) {
                
                HStack {
                    Toggle("", isOn: .constant(true))
                    Text("PDF")
                }
                
                HStack {
                    Toggle("", isOn: .constant(true))
                    Text("PDF (optimized)")
                }
                
                HStack {
                    Toggle("", isOn: .constant(true))
                    Text("JPG")
                }
                Spacer().padding(1)
                
                HStack
                {
                    TextField("Export path", text: $exportPath)
                        .frame(width: 200)
                    
                    Button("Browse") {
                        openFileBrowser()
                    }
                }
            }
            
            Spacer().padding(1)
            
            HStack{
                Button("Export") {
                    self.isPresented = false
                }
                
                Button("Close") {
                    self.isPresented = false
                }
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
            exportPath = panel.url?.absoluteString ?? ""
        }
    }
}




