//
//  LicenseDialog.swift
//  PhotoBook
//
//  Created by Cosmin Mihai on 01.03.2025.
//


import SwiftUI

struct LicenseDialog: View {
    @Binding var isPresented: Bool
    
    var body: some View {
        VStack {
            Text("License")
                .font(.title)
                .padding()
            
            Text("This app is licensed under the MIT License.")
            
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
