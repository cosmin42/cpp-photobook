//
//  MediaList.swift
//  PhotoBook
//
//  Created by Cosmin Mihai on 21.01.2025.
//

import SwiftUI
import PhotosUI

class MediaListModel: ObservableObject
{
    @Published public var list: [MediaItem] = []
    @Published public var selectedItem: MediaItem? = nil
    
    @Published public var onImportImages: (String) -> Void = { _ in }
    @Published public var onRemoveImages: (UInt32) -> Void = { _ in }
    
    @Published public var thumbnailsLocation: String = ""
    
    @Published public var onImagesImported: ([String]) -> Void = { _ in }
    
    public func removeSelected()
    {
        if let index = list.firstIndex(where: {$0.path == selectedItem?.path})
        {
            list.remove(at: index)
        }
    }
    
    public func selectedIndex() -> UInt?
    {
        if let index = list.firstIndex(where: {$0.path == selectedItem?.path})
        {
            return UInt(index)
        }
        return nil
    }
}

struct MediaList: View
{
    @State private var frameSize:CGSize
    @ObservedObject var model: MediaListModel
    
#if os(macOS)
#else
    @State private var selectedItems: [PhotosPickerItem] = []
    @State private var isPickerPresented = false
#endif
    
    init(frameSize: CGSize, model: MediaListModel)
    {
        self.frameSize = frameSize
        self.model = model
    }
    
    var body: some View {
        VStack
        {
            HStack
            {
                
#if os(macOS)
                Button(action: {
                    print("pressed")
                    openImportMediaBrowser()
                }) {
                    Image(systemName: "plus")
                        .scaledToFit()
                        .frame(width: 32, height: 32)
                        .foregroundColor(Color.MainFontColor)
                        .background(Color.clear)
                }
                .frame(alignment: .leading)
                .buttonStyle(PlainButtonStyle())
                .help("Add Folder")
                
                // Remove media button
                Button(action: {
                    print("Remove media tapped")
                    
                    let maybeIndex = model.selectedIndex()
                    if let index = maybeIndex
                    {
                        model.removeSelected()
                        model.onRemoveImages(UInt32(index))
                    }
                }) {
                    Image(systemName: "trash")
                        .scaledToFit()
                        .frame(width: 32, height: 32)
                        .foregroundColor(Color.MainFontColor)
                        .background(Color.clear)
                }
                .frame(alignment: .leading)
                .buttonStyle(PlainButtonStyle())
                .disabled(model.list.isEmpty)
                .help("Remove Group")
                Spacer()
#endif
            }
            ScrollView {
                ForEach(self.model.list.indices, id: \.self) { index in
                    HStack{
                        VStack(alignment:.leading)
                        {
                            Text("\(model.list[index].displayName)")
                                .listRowBackground(Color.PrimaryColor)
                                .font(.headline)
                            Text("\(model.list[index].imagesCount) images")
                                .font(.subheadline)
                        }
                        .background(Color.PrimaryColor)
                        .padding(8)
                        .frame(height: 36, alignment: .leading)
                        .padding(8)
                        .simultaneousGesture(
                            TapGesture().onEnded {
                                model.selectedItem = model.list[index]
                            }
                        )
                        Spacer()
                    }
                    .overlay(content: {
                        if model.selectedItem?.path == model.list[index].path
                        {
                            RoundedRectangle(cornerRadius: 0)
                                .stroke(Color.ButtonPointerOverWhenSelected, lineWidth: 1)
                        }
                    })
                }
                .scrollIndicators(.hidden)
                .background(Color.black.mix(with: Color.BorderColor, by: 0.5))
                Spacer()
            }
#if !os(macOS)
            HStack
            {
                Image(systemName: "plus")
                    .scaledToFit()
                    .frame(width: 56, height: 56)
                    .foregroundColor(Color.MainFontColor)
                    .background(Color.PrimaryColorIPad)
                    .cornerRadius(8)
                    .padding()
                    .frame(alignment: .leading)
                    .buttonStyle(PlainButtonStyle())
                    .help("Add Folder")
                    .photosPicker(isPresented: $isPickerPresented,
                                  selection: $selectedItems,
                                  matching: .images,
                                  photoLibrary: .shared())
                    .simultaneousGesture(
                        TapGesture().onEnded {
                            isPickerPresented = true
                        }
                    )
                    .onChange(of: selectedItems) { newItems in
                        Task {
                            var imagesPaths: [String] = []
                            for item in newItems {
                                if let data = try? await item.loadTransferable(type: Data.self),
                                   let image = UIImage(data: data) {
                                    let base = URL(fileURLWithPath: self.model.thumbnailsLocation)
                                    // TODO: Remove this image after it is resized
                                    let combined = base.appendingPathComponent(UUID().uuidString + ".jpg")
                                    
                                    saveUIImageAsJPEG(image, fileURL: combined, quality: 1.0)
                                    
                                    imagesPaths.append(combined.path)
                                }
                            }
                            model.onImagesImported(imagesPaths)
                        }
                    }
                
                // Remove media button
                Button(action: {
                    print("Remove media tapped")
                    
                    let maybeIndex = model.selectedIndex()
                    if let index = maybeIndex
                    {
                        model.removeSelected()
                        model.onRemoveImages(UInt32(index))
                    }
                }) {
                    Image(systemName: "trash")
                        .scaledToFit()
                        .frame(width: 56, height: 56)
                        .foregroundColor(Color.MainFontColor)
                        .background(Color.RemoveButtonBackground)
                        .cornerRadius(8)
                }
                .frame(alignment: .leading)
                .buttonStyle(PlainButtonStyle())
                .disabled(model.list.isEmpty)
                .help("Remove Group")
                .padding()
                
                Spacer()
            }
            
#endif
        }
        .background(Color.black.mix(with: Color.BorderColor, by: 0.5))
        .frame(alignment: .top)
        .tag(0)
        .scrollIndicators(.hidden)
    }
    
#if os(macOS)
    private func openImportMediaBrowser() {
        
        let panel = NSOpenPanel()
        panel.canChooseFiles = false
        panel.canChooseDirectories = true
        panel.allowsMultipleSelection = false
        
        panel.begin { response in
            if response == .OK, let url = panel.url {
                print("Selected folder URL (macOS): \(url)")
                model.onImportImages(url.absoluteString)
            }
            else if response == .cancel {
                print("Folder picker was cancelled")
            }
        }
    }
#else
    func saveImageToDisk(image: UIImage, fileURL: URL) -> Bool {
        guard let imageData = image.pngData() else {
            print("Failed to convert UIImage to PNG data")
            return false
        }
        
        do {
            try imageData.write(to: fileURL)
            print("Image saved at: \(fileURL)")
            return true
        } catch {
            print("Error saving image: \(error)")
            return false
        }
    }
    
    func saveUIImageAsJPEG(_ image: UIImage, fileURL: URL, quality: CGFloat = 1.0) {
        guard let data = image.jpegData(compressionQuality: quality) else {
            print("Failed to convert UIImage to JPEG data.")
            return
        }
        
        do {
            try data.write(to: fileURL)
            print("Image saved to: \(fileURL)")
        } catch {
            print("Error saving image: \(error)")
        }
    }
    
    
    
#endif
}
