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
    
    private var tabViewRatio = 0.38
    
    init(frameSize: CGSize, model: MediaListModel)
    {
        self.frameSize = frameSize
        self.model = model
    }
    
    var body: some View {
        VStack(alignment:.leading, spacing: 0)
        {
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
        .frame(alignment: .top)
        .tag(0)
        .tabItem {
            Label("Media List", systemImage: "house.fill")
        }
        .scrollIndicators(.hidden)
    }
}
