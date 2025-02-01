//
//  UnstagedPhotoLine.swift
//  PhotoBook
//
//  Created by Cosmin Mihai on 21.01.2025.
//

import SwiftUI

class UnstagedPhotoLineModel: ObservableObject
{
    @Published public var list: [FrontendImage] = []
    @Published var selectedIndices: [Int] = []
}

struct UnstagedPhotoLine: View
{
    @ObservedObject var model: UnstagedPhotoLineModel
    @Binding var canvasImage: FrontendImage?
    @Binding var mediaListModel: MediaListModel
    @Binding var stagedPhotoLineModel: StagedPhotoLineModel
    
    
    var body: some View {
        ScrollView(.horizontal, showsIndicators: false) {
            HStack {
                ForEach(self.model.list.indices, id: \.self) { index in
                    if let fileName = self.model.list[index].resources().small
                    {
                        if let nsImage = NSImage(contentsOfFile: fileName) {
                            Image(nsImage: nsImage)
                                .cornerRadius(10)
                                .frame(height: 80)
                                .overlay(
                                    RoundedRectangle(cornerRadius: 10)
                                        .stroke(model.selectedIndices.contains(index) ? Color.white : Color.clear, lineWidth: 1)
                                )
                                .padding(4)
                                .onTapGesture {
                                    self.canvasImage = model.list[index]
                                    stagedPhotoLineModel.list.removeAll()
                                    if model.selectedIndices.contains(index)
                                    {
                                        model.selectedIndices.removeAll { $0 == index }
                                    }
                                    else
                                    {
                                        model.selectedIndices.append(index)
                                    }
                                }
                                .onDrag {
                                    NSItemProvider(object: UPLIdentifier(row:mediaListModel.selectedIndex(), indices:model.selectedIndices.map { UInt($0) }))
                                }
                        } else {
                            Text("Image not found")
                        }
                    }
                }
            }
            .padding(.horizontal)
            .frame(minHeight:80)
            .border(Color.BorderColor, width: 1)
        }
    }
}
