//
//  DraftPhotoLine.swift
//  PhotoBook
//
//  Created by Cosmin Mihai on 08.03.2025.
//

import SwiftUI

struct DraftPhotoLine: View
{
    @State var frameSize:CGSize
    @ObservedObject var model: DPLModel
    @ObservedObject var photoLinesModel: PhotoLinesModel
    @Binding var multipleSelectionEnabled: Bool
    @Binding var canvasImage: FrontendImage?
    
    var body: some View
    {
        ScrollView(.horizontal, showsIndicators: false) {
            HStack {
                VStack{
                    Text("Draft")
                        .padding(2)
                    Spacer()
                }
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
                                .background(GeometryReader { geo in
                                    Color.clear.preference(
                                        key: ItemFramesKey.self,
                                        value: [IndexedFrame(index: index, frame: geo.frame(in: .global))]
                                    )
                                })
                                .onTapGesture {
                                    photoLinesModel.updatePhotoLineFocus(PhotoLineType.Draft)
                                    if model.selectedIndices.contains(index)
                                    {
                                        if multipleSelectionEnabled
                                        {
                                            model.selectedIndices.removeAll { $0 == index }
                                        }
                                    }
                                    else
                                    {
                                        if multipleSelectionEnabled
                                        {
                                            model.selectedIndices.append(index)
                                        }
                                        else
                                        {
                                            model.selectedIndices.removeAll()
                                            model.selectedIndices.append(index)
                                        }
                                    }
                                    if model.selectedIndices.isEmpty
                                    {
                                        self.canvasImage = nil
                                    }
                                    else if model.selectedIndices.contains(index)
                                    {
                                        self.canvasImage = model.list[index]
                                    }
                                    else
                                    {
                                        self.canvasImage = model.list.first
                                    }
                                }
                                .onDrag {
                                    NSItemProvider(object: UPLIdentifier(row:nil, indices:model.selectedIndices.map { UInt($0) }))
                                }
                        } else {
                            Text("Image not found")
                        }
                    }
                }
            }
            .onPreferenceChange(ItemFramesKey.self) { frames in
                model.itemFrames = frames.sorted(by: { $0.index < $1.index }).map { $0.frame }
            }
        }
        .frame(width: frameSize.width, height:82, alignment: .leading)
        .cornerRadius(10)
        .border(Color.BorderColor, width: 1)
    }
}
