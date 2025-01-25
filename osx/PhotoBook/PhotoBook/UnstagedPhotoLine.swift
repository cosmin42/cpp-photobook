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
    
    init(model: UnstagedPhotoLineModel)
    {
        self.model = model
    }
    
    var body: some View {
        ScrollView(.horizontal, showsIndicators: false) {
            HStack {
                ForEach(self.model.list, id: \.self) { item in
                    if let fileName = item.resources().small
                    {
                        if let nsImage = NSImage(contentsOfFile: fileName) {
                            Image(nsImage: nsImage)
                                .frame(height: 80)
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
