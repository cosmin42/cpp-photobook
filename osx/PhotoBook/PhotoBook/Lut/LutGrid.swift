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
            HStack
            {
                Spacer()
                    .frame(width: 10)
                TextField("Search", text: $model.filterText)
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
                
                Spacer()
            }
            .frame(alignment:.leading)
            .padding(4)
            
            LazyVGrid(columns: model.columns, spacing: 10) {
                let filteredImages = self.model.images.filter{
                    if model.filterText.isEmpty { return true }
                    return $0.name.contains(model.filterText)
                }
                ForEach(filteredImages.indices, id: \.self) { index in
                    if let fileName = filteredImages[index].path
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
                                Text(filteredImages[index].name)
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
