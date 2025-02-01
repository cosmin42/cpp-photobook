//
//  StagedPhotoLine.swift
//  PhotoBook
//
//  Created by Cosmin Mihai on 29.01.2025.
//

import SwiftUI

class StagedPhotoLineModel: ObservableObject
{
    @Published public var list: [FrontendImage] = []
    @State var selectedIndices: [Int] = []
}

struct StagedPhotoLine: View
{
    @ObservedObject var model: StagedPhotoLineModel
    @Binding var canvasImage: FrontendImage?
    @Binding var unstagedPhotoLineModel: UnstagedPhotoLineModel
    
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
                                        .stroke(model.selectedIndices.contains(index) ? Color.yellow : Color.clear, lineWidth: 3)
                                )
                                .onTapGesture {
                                    self.canvasImage = model.list[index]
                                    unstagedPhotoLineModel.list.removeAll()
                                    if model.selectedIndices.contains(index)
                                    {
                                        model.selectedIndices.removeAll { $0 == index }
                                    }
                                    else
                                    {
                                        model.selectedIndices.append(index)
                                    }
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
