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
    
    @State var tabViewRatio = 0.5
    @State var selectedTab: Int = 0
    
    @State private var showImportMediaPicker = false
    
    @State private var uplModel:UnstagedPhotoLineModel = UnstagedPhotoLineModel()
    
    @State private var mediaListModel:MediaListModel
    
    init(navigationPath:Binding<[String]>, photobook: Photobook)
    {
        _photobook = State(initialValue: photobook)
        _navigationPath = navigationPath
        
        mediaListModel = MediaListModel(photobook: photobook)
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
                    
                    // Remove media button
                    Button(action: {
                        print("Remove media tapped")
                        mediaListModel.removeSelected()
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
                    .disabled(mediaListModel.list.isEmpty)
                    
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
                            // Media list
                            MediaList(frameSize: geometry.size, model: self.mediaListModel)
                                .onReceive(mediaListModel.$selectedItem)
                            {
                                newValue in
                                if let newValue = newValue {
                                    // TODO: replace this with a method
                                    if let rowIndex = mediaListModel.list.firstIndex(where: {$0.path == newValue.path})
                                    {
                                        let rowSize:UInt32 = self.photobook.projectManagementService().unstagedImagesRepo().rowSize(UInt32(rowIndex))
                                        for i in 0..<rowSize {
                                            let image = self.photobook.projectManagementService().unstagedImagesRepo().image(UInt32(rowIndex), index:i)
                                            if let unwrappedImage = image
                                            {
                                                if i >= self.uplModel.list.count
                                                {
                                                    self.uplModel.list.append(unwrappedImage)
                                                }
                                                else
                                                {
                                                    self.uplModel.list[Int(i)] = unwrappedImage
                                                }
                                            }
                                        }
                                        if (rowSize < uplModel.list.count)
                                        {
                                            uplModel.list.removeLast(Int(UInt32(uplModel.list.count) - UInt32(rowSize)))
                                        }
                                    }
                                } else {
                                    self.uplModel.list.removeAll()
                                }
                            }
                            
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
                    
                    UnstagedPhotoLine(model: uplModel)
                    
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
        self.mediaListModel.list.append(MediaItem(path:root, displayName: url.lastPathComponent))
        self.mediaListModel.selectedItem = self.mediaListModel.list.last
    }
    
    func onImageUpdated(root: String, row:UInt, index:UInt)
    {
        if let selectedRowIndex = mediaListModel.list.firstIndex(where: {$0.path == root})
        {
            if (row == selectedRowIndex)
            {
                let image = self.photobook.projectManagementService().unstagedImagesRepo().image(UInt32(row), index:UInt32(index))
                if let unwrappedImage = image
                {
                    self.uplModel.list[Int(index)] = unwrappedImage
                }
            }
        }
    }
}

