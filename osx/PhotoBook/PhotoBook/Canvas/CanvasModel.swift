//
//  CanvasModel.swift
//  PhotoBook
//
//  Created by Cosmin Mihai on 25.01.2025.
//

import SwiftUI

class CanvasModel: ObservableObject
{
    @Published public var mainImage:FrontendImage? = nil
#if os(macOS)
    @Published public var maybeProcessedImage:NSImage? = nil
#else
    @Published public var maybeProcessedImage:UIImage? = nil
#endif
    @Published public var pendingLUT:Bool = false
    @Published public var processedImageInfo: (String, String) = ("", "")
    @Published public var onLeftClick: () -> Void = {}
    @Published public var onRightClick: () -> Void = {}
}
