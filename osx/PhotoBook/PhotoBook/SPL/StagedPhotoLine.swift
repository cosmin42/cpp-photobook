//
//  StagedPhotoLine.swift
//  PhotoBook
//
//  Created by Cosmin Mihai on 29.01.2025.
//

import SwiftUI
import UniformTypeIdentifiers

struct IndexedFrame: Equatable {
    let index: Int
    let frame: CGRect
}

struct ItemFramesKey: PreferenceKey {
    static var defaultValue: [IndexedFrame] = []
    
    static func reduce(value: inout [IndexedFrame], nextValue: () -> [IndexedFrame]) {
        value.append(contentsOf: nextValue())
    }
}

class StagedPhotoLineModel: ObservableObject
{
    @Published public var list: [FrontendImage] = []
    @Published public var selectedIndices: [Int] = []
    @Published public var itemFrames: [CGRect] = []

    public func move(fromOffsets:IndexSet, toOffset:UInt?)
    {
        if let toOffset = toOffset
        {
            DispatchQueue.main.async {
                self.list.move(fromOffsets: fromOffsets, toOffset: Int(toOffset))
                self.itemFrames.move(fromOffsets: fromOffsets, toOffset: Int(toOffset))
            }
        }
        else
        {
            for index in fromOffsets.sorted(by: >) {
                DispatchQueue.main.async {
                    self.list.move(fromOffsets: [index], toOffset: self.list.count)
                    self.itemFrames.move(fromOffsets: [index], toOffset: self.itemFrames.count)
                }
            }
        }
        self.selectedIndices.removeAll()
    }
    
    public func insert(image: FrontendImage, position:UInt?)
    {
        if let position = position
        {
            self.list.insert(image, at: Int(position))
        }
        else
        {
            self.list.append(image)
        }
    }
    
    public func findPredecessorIndex(at location: CGPoint) -> UInt?
    {
        if itemFrames.isEmpty
        {
            return nil;
        }
        for i in 0..<itemFrames.count
        {
            if location.x < (itemFrames[i].origin.x + (itemFrames[i].width/2))
            {
                return UInt(i)
            }
        }
        return nil
    }
}

struct StagedPhotoLine: View
{
    @State var frameSize:CGSize
    @ObservedObject var model: StagedPhotoLineModel
    @Binding var canvasImage: FrontendImage?
    @Binding var unstagedPhotoLineModel: UnstagedPhotoLineModel
    @Binding var multipleSelectionEnabled: Bool
    
    var body: some View {
        ScrollView(.horizontal, showsIndicators: false) {
            HStack {
                Text("Album")
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
                                    unstagedPhotoLineModel.selectedIndices.removeAll()
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
                                    NSItemProvider(object: SPLIdentifier(indices:model.selectedIndices.map { UInt($0) }))
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
