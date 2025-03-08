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
    @ObservedObject var basicTransformationModel: BasicTransformationModel
    @ObservedObject var lutsModel: LutGridModel
    @State var frameSize: CGSize
    @State var pendingPath: String = ""
    
    var body: some View {
        VStack {
            if model.pendingLUT
            {
                Rectangle()
                    .fill(Color.PrimaryColor)
                    .frame(maxWidth: .infinity, maxHeight: .infinity)
                    .overlay(
                        ProgressView()
                            .progressViewStyle(CircularProgressViewStyle())
                            .scaleEffect(1.0, anchor: .center)
                    )
            }
            else if let processedImage = model.maybeProcessedImage
            {
                Image(nsImage: processedImage)
                    .resizable()
                    .aspectRatio(contentMode: .fit)
                    .saturation(basicTransformationModel.saturationValue)
                    .contrast(basicTransformationModel.contrastValue)
                    .brightness(basicTransformationModel.brightnessValue)
            }
            else if let selectedImage = model.mainImage
            {
                if let fileName = selectedImage.resources().full
                {
                    if let nsImage = NSImage(contentsOfFile: fileName) {
                        Image(nsImage: nsImage)
                            .resizable()
                            .aspectRatio(contentMode: .fit)
                            .saturation(basicTransformationModel.saturationValue)
                            .contrast(basicTransformationModel.contrastValue)
                            .brightness(basicTransformationModel.brightnessValue)
                    }
                }
            }
            else
            {
                Rectangle()
                    .fill(Color.PrimaryColor)
                    .frame(maxWidth: .infinity, maxHeight: .infinity)
                    .overlay(
                        Text("Canvas Area")
                            .foregroundColor(Color.MainFontColor)
                            .font(.headline)
                    )
            }
            HStack {
                Button(action:{
                    model.onLeftClick()
                }){
                    Image(systemName: "arrow.left")
                        .font(.system(size: 24))
                        .foregroundColor(.white)
                        .background(Color.PrimaryColor)
                }
                .buttonStyle(PlainButtonStyle())
                
                Text(model.mainImage?.maybeOriginalName() ?? "")
                    .foregroundColor(Color.MainFontColor)
                    .font(.headline)
                    .padding()
                
                Button(action:{
                    model.onRightClick()
                }){
                    Image(systemName: "arrow.right")
                        .font(.system(size: 24))
                        .foregroundColor(.white)
                        .background(Color.PrimaryColor)
                }
                .buttonStyle(PlainButtonStyle())
            }
        }
        .padding()
        .frame(width: frameSize.width * 0.5)
        .border(Color.BorderColor, width: 1)
    }
}
