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
    
    var body: some View {
        VStack {
            HStack
            {
                Spacer()
                    .frame(width: 10)
                Text("Filter:")
                    .frame(alignment:.leading)
                TextField("", text: $model.filterText)
                    .textFieldStyle(RoundedBorderTextFieldStyle())
                    .frame(width: 100)
                    .padding(.leading, 4)
                
                Button(action: {
                    model.filterText.removeAll()
                }) {
                    Image(systemName: "xmark.circle.fill")
                        .foregroundColor(.gray)
                        .padding(5)
                }
                .buttonStyle(PlainButtonStyle())
                .frame(alignment:.leading)
                
                Button(action: {
                    if let selectedIndex = model.selectedIndex {
                        model.onApply(selectedIndex)
                    }
                }) {
                    Text("Apply")
                        .background(Color.clear)
                }
                .background(Color.ButtonBackgroundColor)
                .foregroundColor(.white)
                .cornerRadius(5)
                .padding(5)
                .disabled(model.makeCollageDisabled)
                
                Spacer()
            }
            .frame(alignment:.leading)
            .padding(4)
            ScrollView {
                LazyVGrid(columns: model.columns, spacing: 10) {
                    let filteredImages = self.model.images.filter{
                        if model.filterText.isEmpty { return true }
                        return $0.name.contains(model.filterText)
                    }
                    ForEach(filteredImages.indices, id: \.self) { index in
                        if let fileName = self.model.images[index].path
                        {
#if os(macOS)
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
                                            self.model.makeCollageDisabled = !self.model.collagePossible()
                                            self.model.previewDisabled = self.model.makeCollageDisabled
                                        }
                                    Text(self.model.images[index].name)
                                }
                            } else {
                                Text("Image not found")
                            }
#endif
                        }
                    }
                }
                .frame(width: frameSize.width * NoirConstants.GoldenRatioPercentHead)
            }
        }
        .frame(alignment:.leading)
        .tag(1)
    }
}
