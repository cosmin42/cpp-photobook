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
    
    @State var photobook: Photobook
    
    init(photobook: Photobook)
    {
        _photobook = State(initialValue: photobook)
    }
    
    public func removeSelected()
    {
        if let index = list.firstIndex(where: {$0.path == selectedItem?.path})
        {
            list.remove(at: index)
            self.photobook.removeImportFolder(selectedItem?.path)
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
    
    private var tabViewRatio = 0.5
    
    init(frameSize: CGSize, model: MediaListModel)
    {
        self.frameSize = frameSize
        self.model = model
    }
    
    var body: some View {
        VStack(alignment:.leading, spacing: 0)
        {
            ForEach(self.model.list.indices, id: \.self) { index in
                VStack(alignment:.leading)
                {
                    Text("\(model.list[index].displayName)")
                        .listRowBackground(Color.PrimaryColor)
                        .font(.headline)
                    Text("988987 images")
                        .font(.subheadline)
                }
                .background(Color.PrimaryColor)
                .padding(8)
                .frame(width: frameSize.width * tabViewRatio * 0.98,  height: 36, alignment: .leading)
                .overlay(content: {
                    if model.selectedItem?.path == model.list[index].path
                    {
                        RoundedRectangle(cornerRadius: 0)
                            .stroke(Color.ButtonPointerOverWhenSelected, lineWidth: 1)
                    }
                })
                .padding(8)
                .onTapGesture(perform: {
                    model.selectedItem = model.list[index]
                })
            }
            .frame(width: frameSize.width * tabViewRatio, alignment: .topLeading)
            .scrollIndicators(.hidden)
            .background(Color.PrimaryColor)
        }
        .frame(maxHeight: .infinity, alignment: .top)
        .tag(0)
        .tabItem {
            Label("Media List", systemImage: "house.fill")
        }
        .scrollIndicators(.hidden)
    }
}
