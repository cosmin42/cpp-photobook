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
    
    init(photobook: Photobook)
    {
        _photobook = State(initialValue: photobook)
    }
    
    var body: some View {
        VStack(spacing: 0) {
            HStack {
                Button(action: {
                    print("Back button tapped")
                    navigateToDashboard = true
                }) {
                    Text("Back")
                        .padding()
                        .foregroundColor(.white)
                        .background(Color.blue)
                        .cornerRadius(8)
                }
            }
            .padding()
        }
        NavigationLink(
            destination: DashboardView(buttonBackgroundColor: Color(red:21.6/100, green:26.3/100, blue:27.5/100), photobook:self.photobook),
            isActive: $navigateToDashboard
        ) {
            EmptyView()
        }
    }
}

