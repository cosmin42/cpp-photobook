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
#if os(macOS)
                Image(nsImage: processedImage)
                    .resizable()
                    .aspectRatio(contentMode: .fit)
                    .saturation(basicTransformationModel.saturationValue)
                    .brightness(basicTransformationModel.brightnessValue)
                    .contrast(basicTransformationModel.contrastValue)
#else
                Image(uiImage: processedImage)
                    .resizable()
                    .aspectRatio(contentMode: .fit)
                    .saturation(basicTransformationModel.saturationValue)
                    .brightness(basicTransformationModel.brightnessValue)
                    .contrast(basicTransformationModel.contrastValue)
#endif
            }
            else if let selectedImage = model.mainImage
            {
                if let fileName = selectedImage.resources().full
                {
#if os(macOS)
                    if let nsImage = NSImage(contentsOfFile: fileName) {
                        Image(nsImage: nsImage)
                            .resizable()
                            .aspectRatio(contentMode: .fit)
                            .saturation(basicTransformationModel.saturationValue)
                            .contrast(basicTransformationModel.contrastValue)
                            .brightness(basicTransformationModel.brightnessValue)
                    }
#else
                    if let uiImage = UIImage(contentsOfFile: fileName) {
                        Image(uiImage: uiImage)
                            .resizable()
                            .aspectRatio(contentMode: .fit)
                            .saturation(basicTransformationModel.saturationValue)
                            .contrast(basicTransformationModel.contrastValue)
                            .brightness(basicTransformationModel.brightnessValue)
                            .gesture(
                                DragGesture(minimumDistance: 30, coordinateSpace: .local)
                                    .onEnded { value in
                                        if value.translation.width < -50 {
                                            model.onLeftClick()
                                        } else if value.translation.width > 50 {
                                            model.onRightClick()
                                        }
                                    }
                            )
                    }
#endif
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
                HStack{
#if EFFECTS_ENABLED
                    
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
#endif
                    
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
                    
#if EFFECTS_ENABLED
                    
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
#endif
                }
#if os(macOS)
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
                        .font(.subheadline)
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
#endif
            }
        }
    }
}
