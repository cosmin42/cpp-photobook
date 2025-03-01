//
//  SPLDragType.swift
//  PhotoBook
//
//  Created by Cosmin Mihai on 01.02.2025.
//

import SwiftUI
import UniformTypeIdentifiers

struct SplDragType: Identifiable, Codable, Transferable {
    let id: UUID
    let name: String
    
    static var transferRepresentation: some TransferRepresentation {
        CodableRepresentation(contentType: .splDragType)
    }
}

extension UTType {
    static var splDragType: UTType {
        UTType(exportedAs: "com.mandelbrotcode.photobook.splDragType")
    }
}
