//
//  ToPaperModel.swift
//  PhotoBook
//
//  Created by Cosmin Mihai on 23.02.2025.
//

import SwiftUI

class ToPaperData: ObservableObject {
    @Published public var image: FrontendImage
    @Published public var resizeType: String
    @Published public var backgroundColor: Color = Color.white
    
    init(image: FrontendImage, resizeType: String) {
        self.image = image
        self.resizeType = resizeType
    }
}

class ToPaperModel: ObservableObject
{
    @Published public var images:[String: ToPaperData] = [:]
    @Published public var showDialog = false
    @Published public var frameSize: CGSize = CGSize(width: 0, height: 0)
    
    // lambda function for ok button callback
    @Published public var onOk: () -> Void = {}
}
