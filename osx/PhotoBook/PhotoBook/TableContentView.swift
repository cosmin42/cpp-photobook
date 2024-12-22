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
        GeometryReader { geometry in
            VStack {
                HStack {
                    TabView {
                        VStack{
                            // Left-side list
                            List(0..<20, id: \.self) { item in
                                Text("Item \(item)")
                            }
                            .frame(width: geometry.size.width * 0.3)
                            
                            Button("Add Media"){}
                            Button("Remove"){}
                        }
                        .tabItem {
                            Label("Media", systemImage: "house")
                        }
                        
                        VStack{
                            // Left-side list
                            List(0..<20, id: \.self) { item in
                                Text("Item \(item)")
                            }
                            .frame(width: geometry.size.width * 0.3)
                        }
                        .tabItem {
                            Label("Collages", systemImage: "house")
                        }
                        
                        VStack{
                            // Left-side list
                            List(0..<20, id: \.self) { item in
                                Text("Item \(item)")
                            }
                            .frame(width: geometry.size.width * 0.3)
                        }
                        .tabItem {
                            Label("LUTs", systemImage: "house")
                        }
                    }
                    
                    // Right-side canvas
                    Rectangle()
                        .fill(Color.gray.opacity(0.2))
                        .frame(maxWidth: .infinity, maxHeight: .infinity)
                        .overlay(
                            Text("Canvas Area")
                                .foregroundColor(.black)
                                .font(.headline)
                        )
                }
                
                // Bottom horizontal lists
                VStack {
                    ScrollView(.horizontal, showsIndicators: false) {
                        HStack {
                            ForEach(0..<10, id: \.self) { item in
                                RoundedRectangle(cornerRadius: 8)
                                    .fill(Color.blue)
                                    .frame(width: 80, height: 80)
                                    .overlay(
                                        Text("Item \(item)")
                                            .foregroundColor(.white)
                                    )
                            }
                        }
                        .padding(.horizontal)
                    }
                    
                    ScrollView(.horizontal, showsIndicators: false) {
                        HStack {
                            ForEach(0..<10, id: \.self) { item in
                                RoundedRectangle(cornerRadius: 8)
                                    .fill(Color.green)
                                    .frame(width: 80, height: 80)
                                    .overlay(
                                        Text("Item \(item)")
                                            .foregroundColor(.white)
                                    )
                            }
                        }
                        .padding(.horizontal)
                    }
                }
                .frame(height: geometry.size.height * 0.3)
            }
        }
    }
}

