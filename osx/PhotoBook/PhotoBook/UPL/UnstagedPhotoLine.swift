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
    {
        didSet {
            onSelectedIndicesChange?(selectedIndices)
        }
    }
    var onSelectedIndicesChange: (([Int]) -> Void)?
}

struct UnstagedPhotoLine: View
{
    @State var frameSize:CGSize
    @ObservedObject var model: UnstagedPhotoLineModel
    @Binding var canvasImage: FrontendImage?
    @Binding var mediaListModel: MediaListModel
    @Binding var stagedPhotoLineModel: StagedPhotoLineModel
    @Binding var multipleSelectionEnabled: Bool
    
    var body: some View {
        ScrollView(.horizontal, showsIndicators: false) {
            HStack {
                Text("Disk")
                    .frame(width:40)
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
                                    self.stagedPhotoLineModel.selectedIndices.removeAll()
                                    
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
                                        self.canvasImage = model.list.randomElement()
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
        }
        .padding(2)
        .frame(width: frameSize.width, height: 82, alignment: .leading)
        .border(Color.BorderColor, width: 1)
    }
}
