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
    @ObservedObject var stagedPhotoLineModel: StagedPhotoLineModel
    @ObservedObject var unstagedPhotoLineModel: UnstagedPhotoLineModel
    @ObservedObject var draftPhotoLineModel: DPLModel
    @State var frameSize: CGSize
    @State var pendingPath: String = ""
    
    var body: some View {
        VStack {
            Spacer()
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
                    .brightness(basicTransformationModel.brightnessValue)
                    .contrast(basicTransformationModel.contrastValue)
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
                        Text("Canvas")
                            .foregroundColor(Color.MainFontColor)
                            .font(.headline)
                    )
            }
            Spacer()
            HStack {
                Spacer().frame(width: 30)
                HStack{
                    Button(action: {
                        basicTransformationModel.imageProcessingType = .Saturation
                    }) {
                        Text("𑗘").font(.system(size: 20))
                    }
                    .background(Color.PrimaryColor)
                    .buttonStyle(PlainButtonStyle())
                    .overlay(
                        RoundedRectangle(cornerRadius: 12)
                            .stroke(basicTransformationModel.imageProcessingType == .Saturation ? Color.white.opacity(0.5) : Color.clear, lineWidth: 2)
                            .frame(width: 24, height:24)
                    )
                    .disabled(stagedPhotoLineModel.selectedIndices.isEmpty && unstagedPhotoLineModel.selectedIndices.isEmpty && draftPhotoLineModel.selectedIndices.isEmpty)
                    .help("Saturation")
                    .padding()
                    
                    Button(action: {
                        basicTransformationModel.imageProcessingType = .Brightness
                    }) {
                        Text("☼").font(.system(size: 20))
                    }
                    .background(Color.PrimaryColor)
                    .buttonStyle(PlainButtonStyle())
                    .overlay(
                        RoundedRectangle(cornerRadius: 12)
                            .stroke(basicTransformationModel.imageProcessingType == .Brightness ? Color.white.opacity(0.5) : Color.clear, lineWidth: 2)
                            .frame(width: 24, height:24)
                    )
                    .disabled(stagedPhotoLineModel.selectedIndices.isEmpty && unstagedPhotoLineModel.selectedIndices.isEmpty && draftPhotoLineModel.selectedIndices.isEmpty)
                    .help("Brightness")
                    .padding()
                    
                    Button(action: {
                        basicTransformationModel.imageProcessingType = .Contrast
                    }) {
                        Text("◑").font(.system(size: 20))
                    }
                    .background(Color.PrimaryColor)
                    .buttonStyle(PlainButtonStyle())
                    .overlay(
                        RoundedRectangle(cornerRadius: 12)
                            .stroke(basicTransformationModel.imageProcessingType == .Contrast ? Color.white.opacity(0.5) : Color.clear, lineWidth: 2)
                            .frame(width: 24, height:24)
                    )
                    .disabled(stagedPhotoLineModel.selectedIndices.isEmpty && unstagedPhotoLineModel.selectedIndices.isEmpty && draftPhotoLineModel.selectedIndices.isEmpty)
                    .help("Contrast")
                    .padding()
                }
                
                Spacer()
                
                HStack{
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
                }.padding(4)
                Spacer()
            }
            Spacer()
        }
        .frame(width: frameSize.width * NoirConstants.GoldenRatioPercentBody)
    }
}
