//
//  PhotoLinesModel.swift
//  PhotoBook
//
//  Created by Cosmin Mihai on 08.03.2025.
//

import SwiftUI

//enum for photo lines type
enum PhotoLineType
{
    case None
    case Staged
    case Unstaged
    case Draft
}

class PhotoLinesModel: ObservableObject
{
    @Published public var currentPhotoLine: PhotoLineType = .None
    @Published public var onPhotoLineFocusChanged: (PhotoLineType) -> Void = {_ in}
    
    public func updatePhotoLineFocus(_ newFocus: PhotoLineType)
    {
        self.currentPhotoLine = newFocus
        self.onPhotoLineFocusChanged(newFocus)
    }
    
}
