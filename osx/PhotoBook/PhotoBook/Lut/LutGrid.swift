//
//  LutGrid.swift
//  PhotoBook
//
//  Created by Cosmin Mihai on 25.01.2025.
//

import SwiftUI

struct LutGrid: View
{
    @State private var frameSize:CGSize
    @ObservedObject var model: LutGridModel
    
    private var tabViewRatio = 0.5
    
    init(frameSize: CGSize, model: LutGridModel)
    {
        self.frameSize = frameSize
        self.model = model
    }
    
    var body: some View {
        ScrollView {
            LazyVGrid(columns: model.columns, spacing: 10) {
                ForEach(self.model.images.indices, id: \.self) { index in
                    if let fileName = self.model.images[index].path
                    {
                        if let nsImage = NSImage(contentsOfFile: fileName) {
                            VStack
                            {
                                Image(nsImage: nsImage)
                                    .resizable()
                                    .aspectRatio(contentMode: .fit)
                                    .frame(height: 80)
                                    .cornerRadius(10)
                                    .overlay(
                                        RoundedRectangle(cornerRadius: 10)
                                            .stroke(model.selectedIndex == index ? Color.white : Color.clear, lineWidth: 1)
                                    )
                                    .padding(4)
                                    .onTapGesture {
                                        self.model.selectedIndex = index
                                    }
                                Text(self.model.images[index].name)
                            }
                        } else {
                            Text("Image not found")
                        }
                    }
                }
            }
            .frame(width: frameSize.width * tabViewRatio)
        }
        .frame(alignment:.leading)
        .tag(2)
        .tabItem {
            Label("Look Up Tables", systemImage: "house.fill")
        }
        
    }
}
