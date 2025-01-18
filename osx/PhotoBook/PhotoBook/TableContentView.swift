//
//  SwiftUIView.swift
//  PhotoBook
//
//  Created by Cosmin Mihai on 29.10.2023.
//

import SwiftUI

struct TableContentView: View, PhotobookUIListener {
    @State private var navigateToDashboard = false
    @State var photobook: Photobook
    @Binding var navigationPath: [String]
    
    @State private var mediaList: [MediaItem] = []
    
    @State var tabViewRatio = 0.5
    @State var selectedTab: Int = 0
    
    @State private var showImportMediaPicker = false

    init(navigationPath:Binding<[String]>, photobook: Photobook)
    {
        _photobook = State(initialValue: photobook)
        _navigationPath = navigationPath
    }
    
    var body: some View {
        GeometryReader { geometry in
            VStack(alignment: .leading) {
                HStack(spacing: 16) {
                    Button(action: {
                        print("Add Media tapped")
                        showImportMediaPicker = true
                    }) {
                        Image(systemName: "plus")
                            .scaledToFit()
                            .frame(width: 32, height: 32)
                            .foregroundColor(Color.MainFontColor)
                            .background(Color.clear)
                    }
                    .sheet(isPresented: $showImportMediaPicker) {
                        ImportMediaPicker(showImportMediaPicker:$showImportMediaPicker, photobook:self.photobook)
                    }
                    .frame(alignment: .leading)
                    .background(Color.PrimaryColor)
                    .buttonStyle(PlainButtonStyle())
                    
                    Button(action: {
                        print("Remove media tapped")
                    }) {
                        Image(systemName: "trash")
                            .scaledToFit()
                            .frame(width: 32, height: 32)
                            .foregroundColor(Color.MainFontColor)
                            .background(Color.clear)
                    }
                    .frame(alignment: .leading)
                    .background(Color.PrimaryColor)
                    .buttonStyle(PlainButtonStyle())
                    
                    Button(action: {
                        print("Save tapped")
                    }) {
                        Image(systemName: "square.and.arrow.down")
                            .scaledToFit()
                            .frame(width: 32, height: 32)
                            .foregroundColor(Color.MainFontColor)
                            .background(Color.clear)
                    }
                    .frame(alignment: .leading)
                    .background(Color.PrimaryColor)
                    .buttonStyle(PlainButtonStyle())
                    
                    Button(action: {
                        print("Preview tapped")
                    }) {
                        Image(systemName: "eye")
                            .scaledToFit()
                            .frame(width: 32, height: 32)
                            .foregroundColor(Color.MainFontColor)
                            .background(Color.clear)
                    }
                    .frame(alignment: .leading)
                    .background(Color.PrimaryColor)
                    .buttonStyle(PlainButtonStyle())
                    
                    Divider()
                        .frame(height: 32)
                        .background(Color.gray)
                    
                    Button(action: {
                        print("Make collage tapped")
                    }) {
                        Image(systemName: "square.grid.2x2")
                            .scaledToFit()
                            .frame(width: 32, height: 32)
                            .foregroundColor(Color.MainFontColor)
                            .background(Color.clear)
                    }
                    .frame(alignment: .leading)
                    .background(Color.PrimaryColor)
                    .buttonStyle(PlainButtonStyle())
                    
                    Button(action: {
                        print("Send tapped")
                    }) {
                        Image(systemName: "paperplane.fill")
                            .scaledToFit()
                            .frame(width: 32, height: 32)
                            .foregroundColor(Color.MainFontColor)
                            .background(Color.clear)
                    }
                    .frame(alignment: .leading)
                    .background(Color.PrimaryColor)
                    .buttonStyle(PlainButtonStyle())
                    
                    Button(action: {
                        print("Help tapped")
                    }) {
                        Image(systemName: "questionmark.circle")
                            .scaledToFit()
                            .frame(width: 32, height: 32)
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
                                selectedTab = 0
                            }){
                                Text("Media")
                            }
                            .frame(alignment:.leading)
                            .buttonStyle(PlainButtonStyle())
                            Divider()
                                .background(Color.gray)
                                .frame(height: 24)
                            Button(action: {
                                selectedTab = 1
                            }){
                                Text("Collages")
                            }
                            .frame(alignment:.leading)
                            .buttonStyle(PlainButtonStyle())
                            Divider()
                                .background(Color.gray)
                                .frame(height: 24)
                            Button(action: {
                                selectedTab = 2
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
                                List(mediaList, id: \.self) { item in
                                    Text("\(item.displayName)")
                                        .listRowBackground(Color.PrimaryColor)
                                }
                                .frame(width: geometry.size.width * tabViewRatio)
                                .scrollIndicators(.hidden)
                            }
                            .frame(alignment:.leading)
                            .tag(0)
                            .scrollIndicators(.hidden)
                            
                            VStack{
                                List(0..<20, id: \.self) { item in
                                    Text("Item \(item)")
                                }
                                .frame(width: geometry.size.width * tabViewRatio)
                            }
                            .frame(alignment:.leading)
                            .tag(1)
                            
                            VStack{
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
        .onAppear()
        {
            PhotoBookApp.pushListener(listener: self)
            self.photobook.makeCollages()
        }
        .onDisappear()
        {
            self.photobook.unloadProject()
            PhotoBookApp.popListener()
        }
        .foregroundColor(Color.MainFontColor)
        .background(Color.PrimaryColor)
    }
    
    func onProjectRead(){}
    func onMetadataUpdated(focusedName: String){}
    
    func onMappingFinished(root: String){
        let url = URL(fileURLWithPath: root)
        self.mediaList.append(MediaItem(path:root, displayName: url.lastPathComponent))
    }
}

