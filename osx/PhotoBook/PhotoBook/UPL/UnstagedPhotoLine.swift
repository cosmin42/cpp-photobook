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
}

struct UnstagedPhotoLine: View
{
    @ObservedObject var model: UnstagedPhotoLineModel
    @Binding var canvasImage: FrontendImage?
    @Binding var mediaListModel: MediaListModel
    @State var selectedIndices: [Int] = []
    
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
                                        .stroke(selectedIndices.contains(index) ? Color.yellow : Color.clear, lineWidth: 3)
                                )
                                .onTapGesture {
                                    self.canvasImage = model.list[index]
                                    if selectedIndices.contains(index)
                                    {
                                        selectedIndices.removeAll { $0 == index }
                                    }
                                    else
                                    {
                                        selectedIndices.append(index)
                                    }
                                }
                                .onDrag {
                                    NSItemProvider(object: UPLIdentifier(row:mediaListModel.selectedIndex(), indices:[UInt(index)]))
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
