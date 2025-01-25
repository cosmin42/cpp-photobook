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
                ForEach(model.images, id: \.self) { imageName in
                    Text("Item x")
                        .frame(height:80)
                }
            }
            .frame(width: frameSize.width * tabViewRatio)
        }
        .frame(alignment:.leading)
        .tag(1)
    }
}
