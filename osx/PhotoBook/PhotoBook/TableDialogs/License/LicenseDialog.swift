//
//  LicenseDialog.swift
//  PhotoBook
//
//  Created by Cosmin Mihai on 01.03.2025.
//


import SwiftUI

struct LicenseDialog: View {
    @Binding var isPresented: Bool
    @ObservedObject var model: LicenseModel
    
    var body: some View {
        VStack {
            Text("License")
                .font(.title)
                .padding()
            
            Text(model.text)
            
            Button("Close") {
                self.isPresented = false
            }
            .background(Color.RemoveButtonBackground)
            .foregroundColor(Color.MainFontColor)
            .cornerRadius(8)
        }
        .padding()
    }
}
