//
//  AboutDialog.swift
//  PhotoBook
//
//  Created by Cosmin Mihai on 01.03.2025.
//


import SwiftUI

struct AboutDialog: View {
    @Binding var isPresented: Bool
    
    var body: some View {
        VStack {
            Text("About")
                .font(.title)
                .padding()
            
            Text("This app is a simple photo editing app.")
                .padding()
            
            Button("Close") {
                self.isPresented = false
            }
            .padding()
        }
        .padding()
    }
}
