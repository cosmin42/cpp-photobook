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
    case Staged
    case Unstaged
    case Draft
}

class PhotoLinesModel: ObservableObject
{
    @Published public var onPhotoLineFocusChanged: (PhotoLineType) -> Void = {_ in}
}
