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
    @State var selectedIndex: Int = -1
    
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
                                        .stroke(selectedIndex == index ? Color.yellow : Color.clear, lineWidth: 3)
                                )
                                .onTapGesture {
                                    self.canvasImage = model.list[index]
                                    selectedIndex = index
                                }
                        } else {
                            Text("Image not found")
                        }
                    }
                }
            }
            .padding(.horizontal)
        }
    }
}
