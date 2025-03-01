//
//  SubscribeDialog.swift
//  PhotoBook
//
//  Created by Cosmin Mihai on 01.03.2025.
//


import SwiftUI

struct SubscribeDialog: View {
    
    @Binding var isPresented: Bool
    
    var body: some View {
        VStack {
            Text("Subscribe")
                .font(.title)
            Spacer().padding(1)
            Text("Subscribe to PB Noir newsletter to get the latest updates.")
            Spacer().padding(1)
            HStack{
                Text("Email:")
                TextField("Email", text: .constant(""))
                    .textFieldStyle(RoundedBorderTextFieldStyle())
                    .frame(width: 200)
                Button("Subscribe") {
                    self.isPresented = false
                }
            }
        }
        .padding()
    }
}



