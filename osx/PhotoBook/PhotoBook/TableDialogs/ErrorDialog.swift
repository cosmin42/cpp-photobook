//
//  ErrorDialog.swift
//  PhotoBook
//
//  Created by Cosmin Mihai on 01.03.2025.
//


import SwiftUI

struct ErrorDialog: View {
    
    @Binding var isPresented: Bool
    
    var body: some View {
        VStack {
            Text("Error")
                .font(.title)
                .padding()
            
            HStack {
                Image(systemName: "exclamationmark.triangle")
                    .resizable()
                    .frame(width: 50, height: 50)
                    .foregroundColor(Color.red)
                
                Text("An error occurred.")
            }
            
            Button("Ok") {
                self.isPresented = false
            }
            .background(Color.RemoveButtonBackground)
            .foregroundColor(Color.MainFontColor)
            .cornerRadius(8)
        }
        .padding()
    }
}
