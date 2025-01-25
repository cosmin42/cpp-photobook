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
        VStack
        {
            List(model.list, id: \.self, selection: $model.selectedItem) { item in
                HStack
                {
                    Text("\(item.displayName)")
                        .listRowBackground(Color.PrimaryColor)
                        .font(.title)
                }
                .frame(height: 36)
            }
            .frame(width: frameSize.width * tabViewRatio)
            .scrollIndicators(.hidden)
        }
        .frame(alignment:.leading)
        .tag(0)
        .scrollIndicators(.hidden)
    }
}
