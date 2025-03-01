//
//  BasicTransformationModel.swift
//  PhotoBook
//
//  Created by Cosmin Mihai on 01.03.2025.
//

class BasicTransformationModel: ObservableObject
{
    @Published public var saturationValue: Double = 1.0
    @Published public var contrastValue: Double = 1.0
    @Published public var brightnessValue: Double = 0.0
    
    @Published public var imageProcessingType: SimpleImageProcessingType = .None
    
    public let saturationRange = 0.0...2.0
    public let contrastRange = 0.0...2.0
    public let brightnessRange = -1.0...1.0
    
    func reset()
    {
        self.saturationValue = 1.0
        self.contrastValue = 1.0
        self.brightnessValue = 0.0
        self.imageProcessingType = .None
    }
}
