//
//  ErrorDialog.swift
//  PhotoBook
//
//  Created by Cosmin Mihai on 01.03.2025.
//


import SwiftUI

struct ErrorDialog: View {
    
    @Binding var isPresented: Bool
    @ObservedObject var model: ErrorModel
    
    var body: some View {
        VStack {
            HStack {
                Image(systemName: "exclamationmark.triangle")
                    .foregroundColor(Color.red)
                Text("Error")
                    .font(.title)
            }

            Text(model.description)
                .padding()
            
            Button("Ok") {
                self.isPresented = false
            }
            .background(Color.RemoveButtonBackground)
            .foregroundColor(Color.MainFontColor)
            .cornerRadius(8)
        }
        .frame(width: 300)
        .padding()
    }
}
