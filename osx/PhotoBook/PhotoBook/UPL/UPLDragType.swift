//
//  UPLDragType.swift
//  PhotoBook
//
//  Created by Cosmin Mihai on 01.02.2025.
//

import SwiftUI
import UniformTypeIdentifiers

struct UplDragType: Identifiable, Codable, Transferable {
    let id: UUID
    let name: String
    
    static var transferRepresentation: some TransferRepresentation {
        CodableRepresentation(contentType: .uplDragType)
    }
}

extension UTType {
    static var uplDragType: UTType {
        UTType(exportedAs: "com.mandelbrotcode.photobook.uplDragType")
    }
}
