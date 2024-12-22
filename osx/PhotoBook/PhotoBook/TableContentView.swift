//
//  SwiftUIView.swift
//  PhotoBook
//
//  Created by Cosmin Mihai on 29.10.2023.
//

import SwiftUI

struct TableContentView: View {
    @State private var navigateToDashboard = false
    @State var photobook: Photobook
    @Binding var navigationPath: [String]
    
    init(navigationPath:Binding<[String]>, photobook: Photobook)
    {
        _photobook = State(initialValue: photobook)
        _navigationPath = navigationPath
    }
    
    var body: some View {
        VStack(spacing: 0) {
            HStack {
                Button(action: {
                    print("Back button tapped")
                    navigateToDashboard = true
                    navigationPath.removeLast()
                }) {
                    Text("Back")
                        .padding()
                        .foregroundColor(.white)
                        .background(Color.blue)
                        .cornerRadius(8)
                }
                .frame(width: 500, height: 600)
            }
            .frame(alignment: .topLeading)
        }
        .frame(alignment: .topLeading)
    }
}

