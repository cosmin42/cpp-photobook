//
//  CollagesGrid.swift
//  PhotoBook
//
//  Created by Cosmin Mihai on 25.01.2025.
//

import SwiftUI

struct CollagesGrid: View
{
    @State private var frameSize:CGSize
    @ObservedObject var model: CollagesGridModel
    
    private var tabViewRatio = 0.5
    
    init(frameSize: CGSize, model: CollagesGridModel)
    {
        self.frameSize = frameSize
        self.model = model
    }
    
    var body: some View {
        ScrollView {
            LazyVGrid(columns: model.columns, spacing: 10) {
                ForEach(self.model.images, id: \.self) { item in
                    if let fileName = item.path
                    {
                        if let nsImage = NSImage(contentsOfFile: fileName) {
                            Image(nsImage: nsImage)
                                .resizable()
                                .aspectRatio(contentMode: .fit)
                                .frame(height: 80)
                        } else {
                            Text("Image not found")
                        }
                    }
                }
            }
            .frame(width: frameSize.width * tabViewRatio)
        }
        .frame(alignment:.leading)
        .tag(1)
    }
}
