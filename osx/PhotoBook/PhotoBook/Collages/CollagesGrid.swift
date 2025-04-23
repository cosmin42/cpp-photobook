//
//  CollagesGrid.swift
//  PhotoBook
//
//  Created by Cosmin Mihai on 25.01.2025.
//

import SwiftUI

struct CollagesGrid: View
{
    @ObservedObject var model: CollagesGridModel
    @State var frameSize: CGSize
    @Binding var makeCollageDisabled: Bool
    @Binding var previewDisabled: Bool
    
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
                                        self.makeCollageDisabled = !self.model.collagePossible()
                                        self.previewDisabled = self.makeCollageDisabled
                                    }
                                Text(self.model.images[index].name)
                            }
                        } else {
                            Text("Image not found")
                        }
                    }
                }
            }
            .frame(width: frameSize.width * NoirConstants.GoldenRatioPercentHead)
            .padding(4)
        }
        .frame(alignment:.leading)
        .tag(1)
        .tabItem {
            Label("Collages", systemImage: "house.fill")
        }
    }
}
