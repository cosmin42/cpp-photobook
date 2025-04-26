//
//  MediaList.swift
//  PhotoBook
//
//  Created by Cosmin Mihai on 21.01.2025.
//

import SwiftUI

class MediaListModel: ObservableObject
{
    @Published public var list: [MediaItem] = []
    @Published public var selectedItem: MediaItem? = nil
    
    @Published public var onImportImages: (String) -> Void = { _ in }
    @Published public var onRemoveImages: (UInt32) -> Void = { _ in }
    
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
                Button(action: {
                    openImportMediaBrowser()
                }) {
                    Image(systemName: "plus")
                        .scaledToFit()
                        .frame(width: 32, height: 32)
                        .foregroundColor(Color.MainFontColor)
                        .background(Color.clear)
                }
                .frame(alignment: .leading)
                .background(Color.black.mix(with: Color.BorderColor, by: 0.5))
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
                .background(Color.black.mix(with: Color.BorderColor, by: 0.5))
                .buttonStyle(PlainButtonStyle())
                .disabled(model.list.isEmpty)
                .help("Remove Group")
                
                Spacer()
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
                        .onTapGesture(perform: {
                            model.selectedItem = model.list[index]
                        })
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
        }
        .frame(alignment: .top)
        .tag(0)
        .tabItem {
            Label("Media List", systemImage: "house.fill")
        }
        .scrollIndicators(.hidden)
    }
    
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
}
