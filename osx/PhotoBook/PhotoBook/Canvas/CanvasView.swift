//
//  CanvasView.swift
//  PhotoBook
//
//  Created by Cosmin Mihai on 02.02.2025.
//

import SwiftUI

struct CanvasView: View
{
    @ObservedObject var model: CanvasModel
    @State var frameSize: CGSize
    
    var body: some View {
        VStack {
            if let selectedImage = model.mainImage
            {
                if let fileName = selectedImage.resources().full
                {
                    if let nsImage = NSImage(contentsOfFile: fileName) {
                        Image(nsImage: nsImage)
                            .resizable()
                            .aspectRatio(contentMode: .fit)
                    } else {
                        Text("Image not found")
                    }
                }
            }
            else
            {
                Rectangle()
                    .fill(Color.gray.opacity(0.2))
                    .frame(maxWidth: .infinity, maxHeight: .infinity)
                    .overlay(
                        Text("Canvas Area")
                            .foregroundColor(.black)
                            .font(.headline)
                    )
            }
            HStack {
                
                Button(action:{
                    
                }){
                    Image(systemName: "arrow.left")
                        .font(.system(size: 24))
                        .foregroundColor(.white)
                        .background(Color.clear)
                }
                .background(Color.PrimaryColor)
                Text("Image name")
                Button(action:{
                    
                }){
                    Image(systemName: "arrow.right")
                        .font(.system(size: 24))
                        .foregroundColor(.white)
                        .background(Color.clear)
                }
                .background(Color.PrimaryColor)
            }
        }
        .padding()
        .frame(width: frameSize.width * 0.5)
        .border(Color.BorderColor, width: 1)
    }
}
