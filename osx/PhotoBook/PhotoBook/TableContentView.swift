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
    
    @State var tabViewRatio = 0.5
    @State var selectedTab: Int = 0
    
    init(navigationPath:Binding<[String]>, photobook: Photobook)
    {
        _photobook = State(initialValue: photobook)
        _navigationPath = navigationPath
    }
    
    var body: some View {
        GeometryReader { geometry in
            VStack(alignment: .leading) {
                HStack(spacing: 16) { // Adjust spacing as needed
                    Button(action: {
                        print("Add Media tapped")
                    }) {
                        Image(systemName: "plus") // Predefined system icon
                            .scaledToFit() // Optional: Ensures it maintains its aspect ratio
                            .frame(width: 32, height: 32) // Set the size of the icon
                            .foregroundColor(Color.MainFontColor)
                            .background(Color.clear)
                    }
                    .frame(alignment: .leading)
                    .background(Color.PrimaryColor)
                    .buttonStyle(PlainButtonStyle())
                    
                    Button(action: {
                        print("Remove media tapped")
                    }) {
                        Image(systemName: "trash") // Predefined system icon
                            .scaledToFit() // Optional: Ensures it maintains its aspect ratio
                            .frame(width: 32, height: 32) // Set the size of the icon
                            .foregroundColor(Color.MainFontColor)
                            .background(Color.clear)
                    }
                    .frame(alignment: .leading)
                    .background(Color.PrimaryColor)
                    .buttonStyle(PlainButtonStyle())
                    
                    Button(action: {
                        print("Save tapped")
                    }) {
                        Image(systemName: "square.and.arrow.down") // Predefined system icon
                            .scaledToFit() // Optional: Ensures it maintains its aspect ratio
                            .frame(width: 32, height: 32) // Set the size of the icon
                            .foregroundColor(Color.MainFontColor)
                            .background(Color.clear)
                    }
                    .frame(alignment: .leading)
                    .background(Color.PrimaryColor)
                    .buttonStyle(PlainButtonStyle())
                    
                    Button(action: {
                        print("Preview tapped")
                    }) {
                        Image(systemName: "eye") // Predefined system icon
                            .scaledToFit() // Optional: Ensures it maintains its aspect ratio
                            .frame(width: 32, height: 32) // Set the size of the icon
                            .foregroundColor(Color.MainFontColor)
                            .background(Color.clear)
                    }
                    .frame(alignment: .leading)
                    .background(Color.PrimaryColor)
                    .buttonStyle(PlainButtonStyle())
                    
                    Divider()
                        .frame(height: 32) // Set the height of the separator
                        .background(Color.gray) // Set the color of the divider
                    
                    Button(action: {
                        print("Make collage tapped")
                    }) {
                        Image(systemName: "square.grid.2x2") // Predefined system icon
                            .scaledToFit() // Optional: Ensures it maintains its aspect ratio
                            .frame(width: 32, height: 32) // Set the size of the icon
                            .foregroundColor(Color.MainFontColor)
                            .background(Color.clear)
                    }
                    .frame(alignment: .leading)
                    .background(Color.PrimaryColor)
                    .buttonStyle(PlainButtonStyle())
                    
                    Button(action: {
                        print("Send tapped")
                    }) {
                        Image(systemName: "paperplane.fill") // Predefined system icon
                            .scaledToFit() // Optional: Ensures it maintains its aspect ratio
                            .frame(width: 32, height: 32) // Set the size of the icon
                            .foregroundColor(Color.MainFontColor)
                            .background(Color.clear)
                    }
                    .frame(alignment: .leading)
                    .background(Color.PrimaryColor)
                    .buttonStyle(PlainButtonStyle())
                    
                    Button(action: {
                        print("Help tapped")
                    }) {
                        Image(systemName: "questionmark.circle") // Predefined system icon
                            .scaledToFit() // Optional: Ensures it maintains its aspect ratio
                            .frame(width: 32, height: 32) // Set the size of the icon
                            .foregroundColor(Color.MainFontColor)
                            .background(Color.clear)
                    }
                    .frame(alignment: .leading)
                    .background(Color.PrimaryColor)
                    .buttonStyle(PlainButtonStyle())
                }
                .frame(alignment: .leading)
                .background(Color.PrimaryColor)
                HStack {
                    VStack(alignment:.leading) {
                        HStack {
                            Button(action: {
                                selectedTab = 0 // Change the tab to the second one
                            }){
                                Text("Media")
                            }
                            .frame(alignment:.leading)
                            .buttonStyle(PlainButtonStyle())
                            Divider()
                                .background(Color.gray)
                                .frame(height: 24)
                            Button(action: {
                                selectedTab = 1 // Change the tab to the second one
                            }){
                                Text("Collages")
                            }
                            .frame(alignment:.leading)
                            .buttonStyle(PlainButtonStyle())
                            Divider()
                                .background(Color.gray)
                                .frame(height: 24)
                            Button(action: {
                                selectedTab = 2 // Change the tab to the second one
                            }){
                                Text("LUTs")
                            }
                            .frame(alignment:.leading)
                            .buttonStyle(PlainButtonStyle())
                        }
                        .padding()
                        .background(Color.PrimaryColor)
                        TabView(selection: $selectedTab) {
                            VStack{
                                // Left-side list
                                List(0..<50, id: \.self) { item in
                                    Text("Item \(item)")
                                        .listRowBackground(Color.PrimaryColor)
                                }
                                .frame(width: geometry.size.width * tabViewRatio)
                                .scrollIndicators(.hidden)
                            }
                            .frame(alignment:.leading)
                            .tag(0)
                            .scrollIndicators(.hidden)
                            
                            VStack{
                                // Left-side list
                                List(0..<20, id: \.self) { item in
                                    Text("Item \(item)")
                                }
                                .frame(width: geometry.size.width * tabViewRatio)
                            }
                            .frame(alignment:.leading)
                            .tag(1)
                            
                            VStack{
                                // Left-side list
                                List(0..<20, id: \.self) { item in
                                    Text("Item \(item)")
                                }
                                .frame(width: geometry.size.width * tabViewRatio)
                            }
                            .frame(alignment:.leading)
                            .tag(2)
                        }
                        .frame(alignment:.leading)
                        .scrollIndicators(.hidden)
                        .border(Color.BorderColor, width: 1)
                    }
                    .scrollIndicators(.hidden)
                    
                    VStack {
                        // Right-side canvas
                        Rectangle()
                            .fill(Color.gray.opacity(0.2))
                            .frame(maxWidth: .infinity, maxHeight: .infinity)
                            .overlay(
                                Text("Canvas Area")
                                    .foregroundColor(.black)
                                    .font(.headline)
                            )
                        HStack {
                            Button("<"){}
                            Text("Image name")
                            Button(">"){}
                        }
                    }
                    .padding()
                    .border(Color.BorderColor, width: 1)
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
                .border(Color.BorderColor, width: 1)
            }
        }
        .onDisappear()
        {
            self.photobook.unloadProject()
        }
        .foregroundColor(Color.MainFontColor)
        .background(Color.PrimaryColor)
    }
}

