//
//  SwiftUIView.swift
//  PhotoBook
//
//  Created by Cosmin Mihai on 29.10.2023.
//

import SwiftUI
import UniformTypeIdentifiers

struct TableContentView: View, PhotobookUIListener {
    @State private var navigateToDashboard = false
    @State var photobook: Photobook
    @Binding var navigationPath: [String]
    
    @State var tabViewRatio = 0.5
    @State var selectedTab: Int = 0
    
    @State private var showImportMediaPicker = false
    
    @State private var uplModel: UnstagedPhotoLineModel = UnstagedPhotoLineModel()
    @State private var splModel: StagedPhotoLineModel
    @State private var mediaListModel: MediaListModel
    
    @State private var collagesGridModel: CollagesGridModel
    
    @StateObject private var canvasModel: CanvasModel = CanvasModel()
    
    @Binding private var lutGridModel: LutGridModel
    
    @State private var dropIndex:UInt?
    
    init(navigationPath:Binding<[String]>, lutGridModel:Binding<LutGridModel>, photobook: Photobook)
    {
        _photobook = State(initialValue: photobook)
        _navigationPath = navigationPath
        _lutGridModel = lutGridModel
        
        self.splModel = StagedPhotoLineModel(stagedImagesView: photobook.projectManagementService().stagedImages())
        
        self.mediaListModel = MediaListModel(photobook: photobook)
        self.collagesGridModel = CollagesGridModel(photobook: photobook)
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
                            
                            CollagesGrid(frameSize: geometry.size, model: self.collagesGridModel)
                            
                            LutGrid(frameSize: geometry.size, model: self.lutGridModel)
                        }
                        .frame(alignment:.leading)
                        .scrollIndicators(.hidden)
                        .border(Color.BorderColor, width: 1)
                    }
                    .scrollIndicators(.hidden)
                    .frame(width: geometry.size.width * tabViewRatio)
                    
                    VStack {
                        if let selectedImage = canvasModel.mainImage
                        {
                            if let fileName = selectedImage.resources().full
                            {
                                if let nsImage = NSImage(contentsOfFile: fileName) {
                                    Image(nsImage: nsImage)
                                        .resizable()
                                        .aspectRatio(contentMode: .fit)
                                        .frame(height: 300)
                                } else {
                                    Text("Image not found")
                                }
                            }
                        }
                        else
                        {
                            Rectangle()
                                .fill(Color.gray.opacity(0.2))
                                .frame(maxWidth: .infinity, maxHeight: .infinity)
                                .overlay(
                                    Text("Canvas Area")
                                        .foregroundColor(.black)
                                        .font(.headline)
                                )
                        }
                        HStack {
                            Button("<"){}
                            Text("Image name")
                            Button(">"){}
                        }
                    }
                    .padding()
                    .frame(width: geometry.size.width * tabViewRatio)
                    .border(Color.BorderColor, width: 1)
                }
                
                VStack {
                    ScrollView(.horizontal, showsIndicators: false) {
                        HStack {
                            StagedPhotoLine(model: splModel, canvasImage: $canvasModel.mainImage, unstagedPhotoLineModel: $uplModel)
                        }
                        .frame(width:geometry.size.width, height: 80)
                        .border(Color.BorderColor, width: 1)
                        .onDrop(of: [.uplDragType, .splDragType], isTargeted: nil) { providers, location in
                            guard let provider = providers.first else { return false}
                            if provider.hasItemConformingToTypeIdentifier(UTType.uplDragType.identifier) {
                                provider.loadDataRepresentation(forTypeIdentifier: UTType.uplDragType.identifier) { data, error in
                                    if let data = data {
                                        do {
                                            let uplIdentifier = try self.decodeData(data)
                                            var images: [String: FrontendImage] = [:]
                                            for index in uplIdentifier.indices
                                            {
                                                if let row = uplIdentifier.row
                                                {
                                                    let image = self.photobook.projectManagementService().unstagedImagesRepo().image(UInt32(row), index:UInt32(index))
                                                    
                                                    let uuid = UUID()
                                                    images[uuid.uuidString] = image
                                                }
                                                else
                                                {
                                                    assert(false, "Unreachable code")
                                                }
                                            }
                                            self.photobook.mapImages(toSPL: images)
                                            
                                            self.dropIndex = self.splModel.findPredecessorIndex(at:location)
                                            
                                        } catch {
                                            print("Failed to decode dropped data: \(error)")
                                        }
                                    }
                                    else if let error = error {
                                        assert(false, "Unreachable code \(error)")
                                    }
                                    else
                                    {
                                        assert(false, "Unreachable code")
                                    }
                                }
                            }
                            else if provider.hasItemConformingToTypeIdentifier(UTType.splDragType.identifier) {
                                provider.loadDataRepresentation(forTypeIdentifier: UTType.splDragType.identifier) { data, error in
                                    if let data = data {
                                        do {
                                            let splIdentifier = try self.decodeSPLData(data)
                                            let indicesToMove = splIdentifier.indices.map { Int($0) }
                                            let maybeDropIndex = self.splModel.findPredecessorIndex(at:location)
                                            
                                            self.splModel.move(fromOffsets: IndexSet(indicesToMove), toOffset: maybeDropIndex)
                                        } catch {
                                            print("Failed to decode dropped data: \(error)")
                                        }
                                    } else if let error = error {
                                        assert(false, "Unreachable code \(error)")
                                    }
                                    else
                                    {
                                        assert(false, "Unreachable code")
                                    }
                                }
                            }
                            
                            return true
                        }
                    }
                    
                    UnstagedPhotoLine(model: uplModel, canvasImage: $canvasModel.mainImage, mediaListModel: $mediaListModel, stagedPhotoLineModel: $splModel)
                    
                }
                .frame(height: geometry.size.height * 0.3)
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
    func decodeData(_ data: Data) throws -> UPLIdentifier {
        let decoder = JSONDecoder()
        let decoded = try decoder.decode(UPLIdentifier.self, from: data)
        return decoded
    }
    
    func decodeSPLData(_ data: Data) throws -> SPLIdentifier {
        let decoder = JSONDecoder()
        let decoded = try decoder.decode(SPLIdentifier.self, from: data)
        return decoded
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
    
    func onCollageThumbnailsCreated(){
        let collageList = self.photobook.collageTemplatesThumbnailsList()
        if let collageList = collageList
        {
            for i in 0..<collageList.count
            {
                if (self.collagesGridModel.images.count <= i)
                {
                    collagesGridModel.images.append(collageList[i])
                }
                else
                {
                    collagesGridModel.images[i] = collageList[i]
                }
            }
            if (collageList.count < collagesGridModel.images.count)
            {
                collagesGridModel.images.removeLast((collagesGridModel.images.count-collageList.count))
            }
        }
    }
    
    func onImageMapped(imageId: String, image: FrontendImage)
    {
        self.splModel.insert(image: image, position: self.dropIndex)
    }
}
